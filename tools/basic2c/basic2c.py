#!/usr/bin/env python3
"""
Tiny BASIC-to-C translator for sandpiper cross builds.
Supports: line numbers, numeric (double) variables, PRINT, INPUT, LET, IF...THEN <line>,
GOTO, GOSUB/RETURN, FOR/NEXT, END, REM, DIM for 1-D arrays.
Generates standalone C (no external runtime) and can optionally compile with
arm-amd-linux-gnueabi-gcc if available on the target.
"""
from __future__ import annotations
import argparse
import os
import re
import shlex
import subprocess
import sys
from dataclasses import dataclass
from typing import List, Tuple, Dict, Optional, Set

KEYWORDS = {
    "PRINT",
    "INPUT",
    "LET",
    "IF",
    "THEN",
    "GOTO",
    "GOSUB",
    "RETURN",
    "FOR",
    "TO",
    "STEP",
    "NEXT",
    "END",
    "REM",
    "AND",
    "OR",
    "NOT",
    "DIM",
    "OPEN",
    "CLOSE",
    "AS",
    "OUTPUT",
    "APPEND",
}

@dataclass
class Token:
    kind: str  # IDENT, NUMBER, STRING, OP
    value: str

@dataclass
class Statement:
    kind: str
    line: int
    data: dict

class BasicParseError(Exception):
    pass

class BasicProgram:
    def __init__(self, src: str) -> None:
        self.src = src
        self.statements: List[Statement] = []
        self.variables: Set[str] = set()
        self.arrays: Dict[str, int] = {}
        self._for_stack: List[Tuple[str, int]] = []
        self._for_id = 0
        self.return_targets: Set[int] = set()
        self.uses_files: bool = False

    def parse(self) -> None:
        for raw_line in self.src.splitlines():
            stripped = raw_line.strip()
            if not stripped:
                continue
            line_number, remainder = self._split_line_number(stripped)
            tokens = self._tokenize(remainder)
            if not tokens:
                continue
            stmt = self._parse_statement(line_number, tokens)
            if stmt:
                self.statements.append(stmt)

        if self._for_stack:
            raise BasicParseError("Unmatched FOR without NEXT")

    def _split_line_number(self, line: str) -> Tuple[int, str]:
        m = re.match(r"^(\d+)\s+(.*)$", line)
        if not m:
            raise BasicParseError(f"Line is missing a numeric label: {line}")
        return int(m.group(1)), m.group(2).strip()

    def _tokenize(self, line: str) -> List[Token]:
        tokens: List[Token] = []
        i = 0
        while i < len(line):
            ch = line[i]
            if ch.isspace():
                i += 1
                continue
            if ch == '"':
                j = i + 1
                buf = []
                while j < len(line) and line[j] != '"':
                    if line[j] == '\\' and j + 1 < len(line):
                        j += 1
                        buf.append(line[j])
                    else:
                        buf.append(line[j])
                    j += 1
                if j >= len(line) or line[j] != '"':
                    raise BasicParseError("Unterminated string literal")
                tokens.append(Token("STRING", ''.join(buf)))
                i = j + 1
                continue
            if ch.isdigit() or (ch == '.' and i + 1 < len(line) and line[i+1].isdigit()):
                j = i
                seen_dot = False
                while j < len(line) and (line[j].isdigit() or (line[j] == '.' and not seen_dot)):
                    if line[j] == '.':
                        seen_dot = True
                    j += 1
                # exponent part
                if j < len(line) and line[j] in ('e', 'E'):
                    j += 1
                    if j < len(line) and line[j] in ('+', '-'):
                        j += 1
                    while j < len(line) and line[j].isdigit():
                        j += 1
                tokens.append(Token("NUMBER", line[i:j]))
                i = j
                continue
            if ch.isalpha():
                j = i
                while j < len(line) and (line[j].isalnum() or line[j] == '_'):
                    j += 1
                ident = line[i:j].upper()
                tokens.append(Token("IDENT", ident))
                i = j
                continue
            two = line[i:i+2]
            if two in ("<=", ">=", "<>"):
                tokens.append(Token("OP", two))
                i += 2
                continue
            if ch in "+-*/(),;=<>#":
                tokens.append(Token("OP", ch))
                i += 1
                continue
            raise BasicParseError(f"Unknown character: {ch}")
        return tokens

    def _parse_statement(self, line_no: int, tokens: List[Token]) -> Optional[Statement]:
        if tokens[0].kind == "IDENT" and tokens[0].value == "REM":
            return None
        head = tokens[0].value if tokens[0].kind == "IDENT" else None
        if head == "PRINT":
            chan, rest = self._parse_channel_prefix(tokens[1:])
            if chan is not None:
                self.uses_files = True
            items, newline = self._parse_print_list(rest)
            return Statement("PRINT", line_no, {"items": items, "newline": newline, "channel": chan})
        if head == "INPUT":
            chan, rest = self._parse_channel_prefix(tokens[1:])
            if chan is not None:
                self.uses_files = True
            targets = self._parse_input_targets(rest)
            return Statement("INPUT", line_no, {"targets": targets, "channel": chan})
        if head == "LET":
            return self._parse_assignment(line_no, tokens[1:])
        if head and head not in KEYWORDS:
            # Implicit LET
            return self._parse_assignment(line_no, tokens)
        if head == "DIM":
            return self._parse_dim(line_no, tokens)
        if head == "IF":
            return self._parse_if(line_no, tokens)
        if head == "GOTO":
            dest = self._expect_number(tokens, 1)
            return Statement("GOTO", line_no, {"dest": dest})
        if head == "GOSUB":
            dest = self._expect_number(tokens, 1)
            return Statement("GOSUB", line_no, {"dest": dest})
        if head == "RETURN":
            return Statement("RETURN", line_no, {})
        if head == "FOR":
            return self._parse_for(line_no, tokens)
        if head == "NEXT":
            return self._parse_next(line_no, tokens)
        if head == "END":
            return Statement("END", line_no, {})
        if head == "OPEN":
            self.uses_files = True
            return self._parse_open(line_no, tokens)
        if head == "CLOSE":
            self.uses_files = True
            return self._parse_close(line_no, tokens)
        raise BasicParseError(f"Unsupported statement at {line_no}")

    def _parse_assignment(self, line_no: int, tokens: List[Token]) -> Statement:
        name, is_array, idx_expr, eq_pos = self._parse_lhs(tokens)
        if eq_pos >= len(tokens) or tokens[eq_pos].value != "=":
            raise BasicParseError("Assignment must look like VAR = expr")
        expr, _ = self._parse_expression(tokens[eq_pos+1:])
        if is_array:
            self._register_array(name)
            target = f"{name.lower()}[(int)({idx_expr})]"
        else:
            self._register_var(name)
            target = name.lower()
        return Statement("LET", line_no, {"target": target, "expr": expr})

    def _parse_lhs(self, tokens: List[Token]) -> Tuple[str, bool, Optional[str], int]:
        if not tokens or tokens[0].kind != "IDENT":
            raise BasicParseError("Left-hand side must start with a variable")
        name = tokens[0].value
        if len(tokens) > 1 and tokens[1].value == "(":
            j = 2
            depth = 1
            inner: List[Token] = []
            while j < len(tokens):
                if tokens[j].value == "(":
                    depth += 1
                if tokens[j].value == ")":
                    depth -= 1
                    if depth == 0:
                        break
                inner.append(tokens[j])
                j += 1
            if depth != 0:
                raise BasicParseError("Mismatched parentheses in array lhs")
            idx_expr, _ = self._parse_expression(inner)
            return name, True, idx_expr, j + 1
        return name, False, None, 1

    def _parse_target(self, tokens: List[Token], start: int) -> Tuple[str, int]:
        if start >= len(tokens) or tokens[start].kind != "IDENT":
            raise BasicParseError("INPUT expects variable names")
        name = tokens[start].value
        if start + 1 < len(tokens) and tokens[start+1].value == "(":
            j = start + 2
            depth = 1
            inner: List[Token] = []
            while j < len(tokens):
                if tokens[j].value == "(":
                    depth += 1
                if tokens[j].value == ")":
                    depth -= 1
                    if depth == 0:
                        break
                inner.append(tokens[j])
                j += 1
            if depth != 0:
                raise BasicParseError("Mismatched parentheses in INPUT")
            idx_expr, _ = self._parse_expression(inner)
            self._register_array(name)
            return f"{name.lower()}[(int)({idx_expr})]", j - start + 1
        else:
            self._register_var(name)
            return name.lower(), 1

    def _parse_dim(self, line_no: int, tokens: List[Token]) -> Statement:
        # DIM A(10), B(5)
        i = 1
        dims: Dict[str, int] = {}
        while i < len(tokens):
            if tokens[i].kind != "IDENT":
                raise BasicParseError("DIM expects variable names")
            name = tokens[i].value
            if i + 3 >= len(tokens) or tokens[i+1].value != "(" or tokens[i+3].value != ")":
                raise BasicParseError("DIM syntax: DIM A(10),B(5)")
            if tokens[i+2].kind != "NUMBER":
                raise BasicParseError("DIM sizes must be numeric literals")
            size = int(float(tokens[i+2].value))
            if size <= 0:
                raise BasicParseError("DIM size must be positive")
            dims[name.lower()] = size
            i += 4
            if i < len(tokens):
                if tokens[i].value != ",":
                    raise BasicParseError("DIM entries must be comma-separated")
                i += 1
        for k,v in dims.items():
            self.arrays[k] = v
        return Statement("DIM", line_no, {"dims": dims})

    def _parse_channel_prefix(self, tokens: List[Token]) -> Tuple[Optional[int], List[Token]]:
        if tokens and tokens[0].value == "#":
            if len(tokens) < 2 or tokens[1].kind != "NUMBER":
                raise BasicParseError("Channel use requires #<n>")
            ch = int(float(tokens[1].value))
            if len(tokens) >= 3 and tokens[2].value == ",":
                return ch, tokens[3:]
            return ch, tokens[2:]
        return None, tokens

    def _parse_open(self, line_no: int, tokens: List[Token]) -> Statement:
        # OPEN "file" FOR INPUT|OUTPUT|APPEND AS #n
        if len(tokens) < 6 or tokens[1].kind != "STRING":
            raise BasicParseError("OPEN syntax: OPEN \"file\" FOR INPUT|OUTPUT|APPEND AS #n")
        fname = tokens[1].value
        try:
            for_idx = [i for i,t in enumerate(tokens) if t.kind == "IDENT" and t.value == "FOR"][0]
        except IndexError:
            raise BasicParseError("OPEN missing FOR")
        if for_idx + 1 >= len(tokens) or tokens[for_idx+1].kind != "IDENT":
            raise BasicParseError("OPEN missing mode")
        mode = tokens[for_idx+1].value
        if mode not in ("INPUT", "OUTPUT", "APPEND"):
            raise BasicParseError("OPEN mode must be INPUT, OUTPUT, or APPEND")
        try:
            as_idx = [i for i,t in enumerate(tokens) if t.kind == "IDENT" and t.value == "AS"][0]
        except IndexError:
            raise BasicParseError("OPEN missing AS")
        if as_idx + 2 >= len(tokens) or tokens[as_idx+1].value != "#" or tokens[as_idx+2].kind != "NUMBER":
            raise BasicParseError("OPEN AS requires #<n>")
        channel = int(float(tokens[as_idx+2].value))
        return Statement("OPEN", line_no, {"fname": fname, "mode": mode, "channel": channel})

    def _parse_close(self, line_no: int, tokens: List[Token]) -> Statement:
        # CLOSE #n
        if len(tokens) != 3 or tokens[1].value != "#" or tokens[2].kind != "NUMBER":
            raise BasicParseError("CLOSE syntax: CLOSE #<n>")
        channel = int(float(tokens[2].value))
        return Statement("CLOSE", line_no, {"channel": channel})

    def _parse_print_list(self, tokens: List[Token]) -> Tuple[List[Tuple[str, bool]], bool]:
        if not tokens:
            return [], True  # PRINT alone => newline
        parts: List[List[Token]] = []
        buf: List[Token] = []
        depth = 0
        for t in tokens:
            if t.kind == "OP" and t.value == "(":
                depth += 1
            elif t.kind == "OP" and t.value == ")":
                depth -= 1
                if depth < 0:
                    raise BasicParseError("Mismatched parentheses in PRINT")
            if depth == 0 and t.value in (",", ";"):
                parts.append(buf)
                buf = []
                continue
            buf.append(t)
        if depth != 0:
            raise BasicParseError("Mismatched parentheses in PRINT")
        if buf:
            parts.append(buf)
        trailing_sep = tokens[-1].value if tokens and tokens[-1].value in (",", ";") else None
        items: List[Tuple[str, bool]] = []
        for p in parts:
            if not p:
                continue
            expr, is_string = self._parse_expression(p)
            items.append((expr, is_string))
        newline = trailing_sep is None
        return items, newline

    def _parse_input_targets(self, tokens: List[Token]) -> List[str]:
        if not tokens:
            raise BasicParseError("INPUT expects at least one target")
        i = 0
        targets: List[str] = []
        while i < len(tokens):
            target, consumed = self._parse_target(tokens, i)
            targets.append(target)
            i += consumed
            if i < len(tokens):
                if tokens[i].value != ",":
                    raise BasicParseError("INPUT entries must be comma-separated")
                i += 1
        return targets

    def _parse_if(self, line_no: int, tokens: List[Token]) -> Statement:
        try:
            then_idx = [i for i,t in enumerate(tokens) if t.kind == "IDENT" and t.value == "THEN"][0]
        except IndexError:
            raise BasicParseError("IF must contain THEN")
        cond_tokens = tokens[1:then_idx]
        dest_token = tokens[then_idx + 1] if then_idx + 1 < len(tokens) else None
        if dest_token is None or dest_token.kind != "NUMBER":
            raise BasicParseError("THEN must be followed by a line number")
        cond_expr, _ = self._parse_expression(cond_tokens)
        dest = int(dest_token.value)
        return Statement("IF", line_no, {"cond": cond_expr, "dest": dest})

    def _parse_for(self, line_no: int, tokens: List[Token]) -> Statement:
        # FOR i = 1 TO 10 [STEP 2]
        if len(tokens) < 6 or tokens[2].value != "=":
            raise BasicParseError("FOR syntax: FOR var = start TO end [STEP n]")
        var = tokens[1].value
        # Find TO and optional STEP boundaries
        try:
            to_idx = [i for i,t in enumerate(tokens) if t.kind == "IDENT" and t.value == "TO"][0]
        except IndexError:
            raise BasicParseError("FOR must include TO")
        step_idx = None
        for i,t in enumerate(tokens):
            if t.kind == "IDENT" and t.value == "STEP":
                step_idx = i
                break
        start_tokens = tokens[3:to_idx]
        start_expr, _ = self._parse_expression(start_tokens)
        if step_idx is None:
            end_tokens = tokens[to_idx+1:]
            step_expr = "1"
        else:
            end_tokens = tokens[to_idx+1:step_idx]
            step_expr, _ = self._parse_expression(tokens[step_idx+1:])
        end_expr, _ = self._parse_expression(end_tokens)
        self._register_var(var)
        self._for_id += 1
        for_id = self._for_id
        self._for_stack.append((var, for_id))
        return Statement("FOR", line_no, {
            "var": var,
            "start": start_expr,
            "end": end_expr,
            "step": step_expr,
            "id": for_id,
        })

    def _parse_next(self, line_no: int, tokens: List[Token]) -> Statement:
        if not self._for_stack:
            raise BasicParseError("NEXT without matching FOR")
        var, for_id = self._for_stack.pop()
        if len(tokens) >= 2 and tokens[1].kind == "IDENT" and tokens[1].value != var:
            raise BasicParseError("NEXT variable mismatch")
        return Statement("NEXT", line_no, {"var": var, "id": for_id})

    def _parse_expression(self, tokens: List[Token]) -> Tuple[str, bool]:
        if not tokens:
            raise BasicParseError("Missing expression")
        # Shunting-yard to handle precedence
        output: List[Token] = []
        ops: List[Token] = []
        precedence = {
            "NOT": 4,
            "*": 3,
            "/": 3,
            "+": 2,
            "-": 2,
            "<": 1, ">": 1, "<=": 1, ">=": 1, "=": 1, "<>": 1,
            "AND": 0,
            "OR": -1,
        }
        def is_op(tok: Token) -> bool:
            return tok.kind == "OP" or (tok.kind == "IDENT" and tok.value in ("AND", "OR", "NOT"))
        i = 0
        while i < len(tokens):
            tok = tokens[i]
            if tok.kind in ("NUMBER", "STRING"):
                output.append(tok)
            elif tok.kind == "IDENT" and i + 1 < len(tokens) and tokens[i+1].value == "(":
                # Array reference
                name = tok.value
                j = i + 2
                depth = 1
                inner: List[Token] = []
                while j < len(tokens):
                    if tokens[j].value == "(":
                        depth += 1
                    if tokens[j].value == ")":
                        depth -= 1
                        if depth == 0:
                            break
                    inner.append(tokens[j])
                    j += 1
                if depth != 0:
                    raise BasicParseError("Mismatched parentheses in array index")
                idx_expr, _ = self._parse_expression(inner)
                self._register_array(name)
                output.append(Token("IDENT", f"{name.lower()}[(int)({idx_expr})]"))
                i = j  # will be incremented by loop
            elif tok.kind == "IDENT" and tok.value not in ("AND", "OR", "NOT"):
                self._register_var(tok.value)
                output.append(tok)
            elif tok.kind == "IDENT" and tok.value == "NOT":
                ops.append(tok)
            elif tok.kind == "OP" and tok.value == "(":
                ops.append(tok)
            elif tok.kind == "OP" and tok.value == ")":
                while ops and ops[-1].value != "(":
                    output.append(ops.pop())
                if not ops:
                    raise BasicParseError("Mismatched parentheses")
                ops.pop()  # remove (
            elif is_op(tok):
                while ops and ops[-1].value != "(" and precedence.get(ops[-1].value, -100) >= precedence.get(tok.value, -100):
                    output.append(ops.pop())
                ops.append(tok)
            elif tok.value in (",", ";"):
                # Treat as expression separator; should be handled by caller
                break
            else:
                raise BasicParseError(f"Unexpected token in expression: {tok}")
            i += 1
        while ops:
            if ops[-1].value in "()":
                raise BasicParseError("Mismatched parentheses")
            output.append(ops.pop())

        def emit(postfix: List[Token]) -> str:
            stack: List[str] = []
            for t in postfix:
                if t.kind == "NUMBER":
                    stack.append(t.value)
                elif t.kind == "STRING":
                    escaped = t.value.replace('\\', '\\\\').replace('"', '\\"')
                    stack.append(f'"{escaped}"')
                elif t.kind == "IDENT":
                    stack.append(t.value.lower())
                else:
                    if t.value == "NOT":
                        a = stack.pop()
                        stack.append(f"(!({a}))")
                    else:
                        b = stack.pop(); a = stack.pop()
                        op = {
                            "AND": "&&",
                            "OR": "||",
                            "<>": "!=",
                        }.get(t.value, t.value)
                        stack.append(f"({a} {op} {b})")
            if len(stack) != 1:
                raise BasicParseError("Bad expression assembly")
            return stack[0]

        expr_str = emit(output)
        is_string = len(output) == 1 and output[0].kind == "STRING"
        return expr_str, is_string

    def _register_var(self, name: str) -> None:
        if name in KEYWORDS:
            raise BasicParseError(f"{name} is a reserved keyword")
        if name.lower() in self.arrays:
            raise BasicParseError(f"{name} is an array; use indexing")
        self.variables.add(name.lower())

    def _register_array(self, name: str) -> None:
        if name.lower() not in self.arrays:
            raise BasicParseError(f"Array {name} used without DIM")

    def _expect_number(self, tokens: List[Token], idx: int) -> int:
        if idx >= len(tokens) or tokens[idx].kind != "NUMBER":
            raise BasicParseError("Expected line number")
        return int(tokens[idx].value)

    def to_c(self) -> str:
        if not self.statements:
            raise BasicParseError("Program is empty")
        # Precompute possible RETURN targets (next instruction after a GOSUB)
        self.return_targets = set()
        for stmt in self.statements:
            if stmt.kind == "GOSUB":
                self.return_targets.add(self._next_label_number(stmt))
        lines: List[str] = []
        lines.append("#include <stdio.h>")
        lines.append("")
        lines.append("#define GOSUB_STACK_MAX 1024")
        lines.append("int __gosub_stack[GOSUB_STACK_MAX];")
        lines.append("int __gosub_sp = 0;")
        if self.uses_files:
            lines.append("#define FILE_CH_MAX 16")
            lines.append("FILE * __files[FILE_CH_MAX] = {0};")
        lines.append("")
        lines.append("int main(void) {")
        if self.variables:
            decls = ", ".join(sorted(f"{v}=0.0" for v in self.variables))
            lines.append(f"    double {decls};")
        for name, size in sorted(self.arrays.items()):
            lines.append(f"    double {name}[{size}] = {{0}};")
        lines.append("    goto L" + str(self.statements[0].line) + ";")
        lines.append("")
        for stmt in self.statements:
            lines.append(f"L{stmt.line}:")
            emit = getattr(self, f"_emit_{stmt.kind.lower()}")
            emit(lines, stmt)
        lines.append("Lexit:")
        if self.uses_files:
            lines.append("    for (int __i = 0; __i < FILE_CH_MAX; __i++) { if (__files[__i]) fclose(__files[__i]); }")
        lines.append("    return 0;")
        lines.append("}")
        return "\n".join(lines) + "\n"

    def _emit_print(self, out: List[str], stmt: Statement) -> None:
        items = stmt.data["items"]
        newline = stmt.data["newline"]
        channel = stmt.data.get("channel")
        if channel is not None:
            out.append(f"    if (__files[{channel}] == NULL) {{ fprintf(stderr, \"File #{channel} not open\\n\"); return 1; }}")
        if not items and newline:
            if channel is None:
                out.append("    printf(\"\\n\");")
            else:
                out.append(f"    fprintf(__files[{channel}], \"\\n\");")
        else:
            for expr, is_string in items:
                fmt = "%s" if is_string else "%g"
                if channel is None:
                    out.append(f"    printf(\"{fmt}\", {expr});")
                else:
                    out.append(f"    fprintf(__files[{channel}], \"{fmt}\", {expr});")
            if newline:
                if channel is None:
                    out.append("    printf(\"\\n\");")
                else:
                    out.append(f"    fprintf(__files[{channel}], \"\\n\");")
        out.append("    goto L" + self._next_label(stmt) + ";")

    def _emit_dim(self, out: List[str], stmt: Statement) -> None:
        # DIM only affects declarations; at runtime it is a no-op.
        out.append("    goto L" + self._next_label(stmt) + ";")

    def _emit_input(self, out: List[str], stmt: Statement) -> None:
        targets = stmt.data["targets"]
        channel = stmt.data.get("channel")
        if channel is not None:
            out.append(f"    if (__files[{channel}] == NULL) {{ fprintf(stderr, \"File #{channel} not open\\n\"); return 1; }}")
        for t in targets:
            if channel is None:
                out.append(f"    if (scanf(\"%lf\", &{t}) != 1) return 1;")
            else:
                out.append(f"    if (fscanf(__files[{channel}], \"%lf\", &{t}) != 1) return 1;")
        out.append("    goto L" + self._next_label(stmt) + ";")

    def _emit_let(self, out: List[str], stmt: Statement) -> None:
        target = stmt.data["target"]
        expr = stmt.data["expr"]
        out.append(f"    {target} = {expr};")
        out.append("    goto L" + self._next_label(stmt) + ";")

    def _emit_if(self, out: List[str], stmt: Statement) -> None:
        cond = stmt.data["cond"]
        dest = stmt.data["dest"]
        out.append(f"    if ({cond}) goto L{dest};")
        out.append("    goto L" + self._next_label(stmt) + ";")

    def _emit_goto(self, out: List[str], stmt: Statement) -> None:
        dest = stmt.data["dest"]
        out.append(f"    goto L{dest};")

    def _emit_open(self, out: List[str], stmt: Statement) -> None:
        fname = stmt.data["fname"].replace("\\", "\\\\").replace("\"", "\\\"")
        mode = stmt.data["mode"]
        channel = stmt.data["channel"]
        c_mode = {"INPUT": "r", "OUTPUT": "w", "APPEND": "a"}[mode]
        out.append(f"    int __ch = {channel};")
        out.append("    if (__ch < 0 || __ch >= FILE_CH_MAX) { fprintf(stderr, \"Bad channel\\n\"); return 1; }")
        out.append("    if (__files[__ch]) { fclose(__files[__ch]); __files[__ch] = NULL; }")
        out.append(f"    __files[__ch] = fopen(\"{fname}\", \"{c_mode}\");")
        out.append("    if (!__files[__ch]) { perror(\"open\"); return 1; }")
        out.append("    goto L" + self._next_label(stmt) + ";")

    def _emit_close(self, out: List[str], stmt: Statement) -> None:
        channel = stmt.data["channel"]
        out.append(f"    int __ch = {channel};")
        out.append("    if (__ch < 0 || __ch >= FILE_CH_MAX) { fprintf(stderr, \"Bad channel\\n\"); return 1; }")
        out.append("    if (__files[__ch]) { fclose(__files[__ch]); __files[__ch] = NULL; }")
        out.append("    goto L" + self._next_label(stmt) + ";")

    def _emit_gosub(self, out: List[str], stmt: Statement) -> None:
        dest = stmt.data["dest"]
        ret_label = self._next_label_number(stmt)
        out.append("    if (__gosub_sp >= GOSUB_STACK_MAX) { fprintf(stderr, \"GOSUB stack overflow\\n\"); return 1; }")
        out.append(f"    __gosub_stack[__gosub_sp++] = {ret_label};")
        out.append(f"    goto L{dest};")

    def _emit_return(self, out: List[str], stmt: Statement) -> None:
        out.append("    if (__gosub_sp <= 0) { fprintf(stderr, \"RETURN without GOSUB\\n\"); return 1; }")
        out.append("    __gosub_sp--;")
        out.append("    switch (__gosub_stack[__gosub_sp]) {")
        for target in sorted(self.return_targets):
            label = "exit" if target == -1 else str(target)
            out.append(f"    case {target}: goto L{label};")
        out.append("    default: fprintf(stderr, \"Bad RETURN target\\n\"); return 1;")
        out.append("    }")

    def _emit_for(self, out: List[str], stmt: Statement) -> None:
        var = stmt.data["var"].lower()
        start = stmt.data["start"]
        end = stmt.data["end"]
        step = stmt.data["step"]
        fid = stmt.data["id"]
        out.append(f"    {var} = {start};")
        out.append(f"    double __for_end_{fid} = {end};")
        out.append(f"    double __for_step_{fid} = {step};")
        out.append(f"__for_check_{fid}:")
        out.append(f"    if ((__for_step_{fid} >= 0 && {var} > __for_end_{fid}) || (__for_step_{fid} < 0 && {var} < __for_end_{fid})) goto __for_exit_{fid};")
        out.append("    goto L" + self._next_label(stmt) + ";")

    def _emit_next(self, out: List[str], stmt: Statement) -> None:
        var = stmt.data["var"].lower()
        fid = stmt.data["id"]
        out.append(f"__for_next_{fid}:")
        out.append(f"    {var} += __for_step_{fid};")
        out.append(f"    goto __for_check_{fid};")
        out.append(f"__for_exit_{fid}:")
        out.append("    goto L" + self._next_label(stmt) + ";")

    def _emit_end(self, out: List[str], stmt: Statement) -> None:
        if self.uses_files:
            out.append("    for (int __i = 0; __i < FILE_CH_MAX; __i++) { if (__files[__i]) fclose(__files[__i]); }")
        out.append("    return 0;")

    def _next_label(self, stmt: Statement) -> str:
        # Find the next statement or end
        idx = self.statements.index(stmt)
        if idx + 1 < len(self.statements):
            return str(self.statements[idx + 1].line)
        return "exit"  # sentinel

    def _next_label_number(self, stmt: Statement) -> int:
        idx = self.statements.index(stmt)
        if idx + 1 < len(self.statements):
            return self.statements[idx + 1].line
        return -1

def convert_to_c(src_path: str, out_c: str) -> str:
    with open(src_path, "r", encoding="utf-8") as fh:
        program = BasicProgram(fh.read())
    program.parse()
    c_code = program.to_c()
    with open(out_c, "w", encoding="utf-8") as out:
        out.write(c_code)
    return out_c

def compile_c(out_c: str, out_elf: str, cc: str, cflags: str) -> None:
    cmd = [cc] + shlex.split(cflags) + [out_c, "-o", out_elf]
    res = subprocess.run(cmd, capture_output=True, text=True)
    if res.returncode != 0:
        sys.stderr.write(res.stdout)
        sys.stderr.write(res.stderr)
        raise SystemExit(res.returncode)


def main() -> None:
    ap = argparse.ArgumentParser(description="Tiny BASIC to C converter")
    ap.add_argument("input", help="Input BASIC file")
    ap.add_argument("--output-c", default=None, help="Where to write generated C")
    ap.add_argument("--compile", action="store_true", help="Invoke cross-compiler after generation")
    ap.add_argument("--out-elf", default=None, help="Output ELF path when compiling")
    ap.add_argument("--cc", default="arm-amd-linux-gnueabi-gcc", help="Cross compiler to invoke")
    ap.add_argument("--cflags", default="-O2", help="Extra CFLAGS for the compiler")
    args = ap.parse_args()

    in_path = args.input
    base = os.path.splitext(os.path.basename(in_path))[0]
    out_c = args.output_c or base + ".c"
    out_elf = args.out_elf or base + ".elf"

    try:
        convert_to_c(in_path, out_c)
    except BasicParseError as e:
        raise SystemExit(f"Parse error: {e}")

    print(f"Generated C: {out_c}")
    if args.compile:
        compile_c(out_c, out_elf, args.cc, args.cflags)
        print(f"Built ELF: {out_elf}")

if __name__ == "__main__":
    main()

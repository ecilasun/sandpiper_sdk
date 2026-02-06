/*
 * Copyright (C) 2020 Shotaro Uchida <fantom@xmaker.mx>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <quakedef.h>
#include <quakembd.h>

void IN_Init(void)
{
}

void IN_Shutdown(void)
{
}

void IN_Commands(void)
{
}

void IN_Move(usercmd_t *cmd)
{
	mouse_movement_t movement;
	int r;
	static uint32_t old_buttons = 0;

	r = qembd_get_mouse_movement(&movement);
	if (r != 0)
		return;

	if (movement.buttons != old_buttons)
	{
		for (int i = 0; i < 3; i++)
		{
			int mask = 1 << i;
			if ((movement.buttons & mask) != (old_buttons & mask))
			{
				Key_Event(K_MOUSE1 + i, (movement.buttons & mask) != 0);
			}
		}
		old_buttons = movement.buttons;
	}

	movement.x *= sensitivity.value;
	movement.y *= sensitivity.value;

	V_StopPitchDrift();
	cl.viewangles[YAW] -= m_yaw.value * movement.x;
	cl.viewangles[PITCH] += m_pitch.value * movement.y;
	if (cl.viewangles[PITCH] > 80)
		cl.viewangles[PITCH] = 80;
	if (cl.viewangles[PITCH] < -70)
		cl.viewangles[PITCH] = -70;
}


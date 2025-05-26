/* SPDX-License-Identifier: GPL-2.0-or-later */


#ifndef G10_MPI_H
#define G10_MPI_H

#include <linux/types.h>
#include <linux/scatterlist.h>

#define BYTES_PER_MPI_LIMB	(BITS_PER_LONG / 8)
#define BITS_PER_MPI_LIMB	BITS_PER_LONG

typedef unsigned long int mpi_limb_t;
typedef signed long int mpi_limb_signed_t;

struct gcry_mpi {
	int alloced;		
	int nlimbs;		
	int nbits;		
	int sign;		
	unsigned flags;		
	
	
	mpi_limb_t *d;		
};

typedef struct gcry_mpi *MPI;

#define mpi_get_nlimbs(a)     ((a)->nlimbs)
#define mpi_has_sign(a)       ((a)->sign)


MPI mpi_alloc(unsigned nlimbs);
void mpi_clear(MPI a);
void mpi_free(MPI a);
int mpi_resize(MPI a, unsigned nlimbs);

static inline MPI mpi_new(unsigned int nbits)
{
	return mpi_alloc((nbits + BITS_PER_MPI_LIMB - 1) / BITS_PER_MPI_LIMB);
}

MPI mpi_copy(MPI a);
MPI mpi_alloc_like(MPI a);
void mpi_snatch(MPI w, MPI u);
MPI mpi_set(MPI w, MPI u);
MPI mpi_set_ui(MPI w, unsigned long u);
MPI mpi_alloc_set_ui(unsigned long u);
void mpi_swap_cond(MPI a, MPI b, unsigned long swap);


#define MPI_NUMBER_OF_CONSTANTS 6
enum gcry_mpi_constants {
	MPI_C_ZERO,
	MPI_C_ONE,
	MPI_C_TWO,
	MPI_C_THREE,
	MPI_C_FOUR,
	MPI_C_EIGHT
};

MPI mpi_const(enum gcry_mpi_constants no);




enum gcry_mpi_format {
	GCRYMPI_FMT_NONE = 0,
	GCRYMPI_FMT_STD = 1,    
	GCRYMPI_FMT_PGP = 2,    
	GCRYMPI_FMT_SSH = 3,    
	GCRYMPI_FMT_HEX = 4,    
	GCRYMPI_FMT_USG = 5,    
	GCRYMPI_FMT_OPAQUE = 8  
};

MPI mpi_read_raw_data(const void *xbuffer, size_t nbytes);
MPI mpi_read_from_buffer(const void *buffer, unsigned *ret_nread);
int mpi_fromstr(MPI val, const char *str);
MPI mpi_scanval(const char *string);
MPI mpi_read_raw_from_sgl(struct scatterlist *sgl, unsigned int len);
void *mpi_get_buffer(MPI a, unsigned *nbytes, int *sign);
int mpi_read_buffer(MPI a, uint8_t *buf, unsigned buf_len, unsigned *nbytes,
		    int *sign);
int mpi_write_to_sgl(MPI a, struct scatterlist *sg, unsigned nbytes,
		     int *sign);
int mpi_print(enum gcry_mpi_format format, unsigned char *buffer,
			size_t buflen, size_t *nwritten, MPI a);


void mpi_mod(MPI rem, MPI dividend, MPI divisor);


struct barrett_ctx_s;
typedef struct barrett_ctx_s *mpi_barrett_t;

mpi_barrett_t mpi_barrett_init(MPI m, int copy);
void mpi_barrett_free(mpi_barrett_t ctx);
void mpi_mod_barrett(MPI r, MPI x, mpi_barrett_t ctx);
void mpi_mul_barrett(MPI w, MPI u, MPI v, mpi_barrett_t ctx);


int mpi_powm(MPI res, MPI base, MPI exp, MPI mod);


int mpi_cmp_ui(MPI u, ulong v);
int mpi_cmp(MPI u, MPI v);
int mpi_cmpabs(MPI u, MPI v);


int mpi_sub_ui(MPI w, MPI u, unsigned long vval);


void mpi_normalize(MPI a);
unsigned mpi_get_nbits(MPI a);
int mpi_test_bit(MPI a, unsigned int n);
void mpi_set_bit(MPI a, unsigned int n);
void mpi_set_highbit(MPI a, unsigned int n);
void mpi_clear_highbit(MPI a, unsigned int n);
void mpi_clear_bit(MPI a, unsigned int n);
void mpi_rshift_limbs(MPI a, unsigned int count);
void mpi_rshift(MPI x, MPI a, unsigned int n);
void mpi_lshift_limbs(MPI a, unsigned int count);
void mpi_lshift(MPI x, MPI a, unsigned int n);


void mpi_add_ui(MPI w, MPI u, unsigned long v);
void mpi_add(MPI w, MPI u, MPI v);
void mpi_sub(MPI w, MPI u, MPI v);
void mpi_addm(MPI w, MPI u, MPI v, MPI m);
void mpi_subm(MPI w, MPI u, MPI v, MPI m);


void mpi_mul(MPI w, MPI u, MPI v);
void mpi_mulm(MPI w, MPI u, MPI v, MPI m);


void mpi_tdiv_r(MPI rem, MPI num, MPI den);
void mpi_fdiv_r(MPI rem, MPI dividend, MPI divisor);
void mpi_fdiv_q(MPI quot, MPI dividend, MPI divisor);


int mpi_invm(MPI x, MPI a, MPI n);




struct gcry_mpi_point {
	MPI x;
	MPI y;
	MPI z;
};

typedef struct gcry_mpi_point *MPI_POINT;


enum gcry_mpi_ec_models {
	
	MPI_EC_WEIERSTRASS = 0,
	
	MPI_EC_MONTGOMERY,
	
	MPI_EC_EDWARDS
};


enum ecc_dialects {
	ECC_DIALECT_STANDARD = 0,
	ECC_DIALECT_ED25519,
	ECC_DIALECT_SAFECURVE
};


struct mpi_ec_ctx {
	enum gcry_mpi_ec_models model; 
	enum ecc_dialects dialect;     
	int flags;                     
	unsigned int nbits;            

	
	MPI p;         
	MPI a;         
	MPI b;         
	MPI_POINT G;   
	MPI n;         
	unsigned int h;       

	
	MPI_POINT Q;   
	MPI d;         

	const char *name;      

	
	struct {
		struct {
			unsigned int a_is_pminus3:1;
			unsigned int two_inv_p:1;
		} valid; 

		int a_is_pminus3;  

		MPI two_inv_p;

		mpi_barrett_t p_barrett;

		
		MPI scratch[11];

		
		
		
		
	} t;

	
	void (*addm)(MPI w, MPI u, MPI v, struct mpi_ec_ctx *ctx);
	void (*subm)(MPI w, MPI u, MPI v, struct mpi_ec_ctx *ec);
	void (*mulm)(MPI w, MPI u, MPI v, struct mpi_ec_ctx *ctx);
	void (*pow2)(MPI w, const MPI b, struct mpi_ec_ctx *ctx);
	void (*mul2)(MPI w, MPI u, struct mpi_ec_ctx *ctx);
};

void mpi_ec_init(struct mpi_ec_ctx *ctx, enum gcry_mpi_ec_models model,
			enum ecc_dialects dialect,
			int flags, MPI p, MPI a, MPI b);
void mpi_ec_deinit(struct mpi_ec_ctx *ctx);
MPI_POINT mpi_point_new(unsigned int nbits);
void mpi_point_release(MPI_POINT p);
void mpi_point_init(MPI_POINT p);
void mpi_point_free_parts(MPI_POINT p);
int mpi_ec_get_affine(MPI x, MPI y, MPI_POINT point, struct mpi_ec_ctx *ctx);
void mpi_ec_add_points(MPI_POINT result,
			MPI_POINT p1, MPI_POINT p2,
			struct mpi_ec_ctx *ctx);
void mpi_ec_mul_point(MPI_POINT result,
			MPI scalar, MPI_POINT point,
			struct mpi_ec_ctx *ctx);
int mpi_ec_curve_point(MPI_POINT point, struct mpi_ec_ctx *ctx);




static inline unsigned int mpi_get_size(MPI a)
{
	return a->nlimbs * BYTES_PER_MPI_LIMB;
}
#endif 

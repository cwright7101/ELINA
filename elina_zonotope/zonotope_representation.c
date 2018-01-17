#include "zonotope.h"
#include "zonotope_internal.h"
#include "zonotope_representation.h"

/* memory allocation of Zonotope structure */
zonotope_t* zonotope_alloc(elina_manager_t* man, size_t intdim, size_t realdim)
{

    zonotope_internal_t * pr = zonotope_init_from_manager(man, ELINA_FUNID_UNKNOWN);
    zonotope_t* res = (zonotope_t *)malloc(sizeof(zonotope_t));

    res->intdim = intdim;
    res->dims = intdim + realdim;
    res->size = 128;
    res->nsymcons = (elina_dim_t*)calloc(res->size, sizeof(elina_dim_t));
    res->gamma = (elina_interval_t**)calloc(res->size, sizeof(elina_interval_t*));
    res->abs = elina_abstract0_top(pr->manNS, 0, 0);
    res->hypercube = true;
    res->box = elina_interval_array_alloc(intdim + realdim);
    res->gn = 0;
    res->g = NULL;
    res->paf = (zonotope_aff_t**)malloc(res->dims*sizeof(zonotope_aff_t*));
    return res;
}



/* Return a copy of an abstract value, on
 * which destructive update does not affect the initial value. */
zonotope_t* zonotope_copy(elina_manager_t* man, zonotope_t* z)
{
    zonotope_t* res;
    size_t i;
    if(z==NULL){
	return NULL;
    }
    zonotope_internal_t* pr = zonotope_init_from_manager(man, ELINA_FUNID_COPY);
    res = zonotope_alloc(man, z->intdim, (z->dims - z->intdim));
    memcpy((void *)res->paf, (void *)z->paf, z->dims*sizeof(zonotope_aff_t*));
    for(i=0; i<z->dims; i++) {
      elina_interval_set(res->box[i], z->box[i]);
      res->paf[i]->pby++;
    }
    elina_abstract0_free(pr->manNS, res->abs);
    res->abs = elina_abstract0_copy(pr->manNS, z->abs);
    res->size = z->size;
    memcpy((void *)res->nsymcons, (void *)z->nsymcons, (res->size)*sizeof(elina_dim_t));
    //memcpy((void *)res->gamma, (void *)a->gamma, (res->size)*sizeof(ap_interval_t*));

    size_t nsymcons_size = zonotope_noise_symbol_cons_get_dimension(pr, z);
    for (i=0; i<nsymcons_size; i++) {
	if (z->gamma[i]) {
	    if (z->gamma[i] != pr->ap_muu) res->gamma[i] = elina_interval_alloc_set(z->gamma[i]);
	    else res->gamma[i] = pr->ap_muu;
	} else {
          printf("zonotope_copy, unconsistent gamma for Zonotope abstract "
                 "object\n");
        }
    }
    res->hypercube = z->hypercube;
    man->result.flag_best = false;
    man->result.flag_exact = false;

    return res;
}


/* free memory used by abstract value */
void zonotope_free(elina_manager_t* man, zonotope_t* z)
{
    if(z==NULL){
	return;
    }
    zonotope_internal_t * pr = zonotope_init_from_manager(man, ELINA_FUNID_FREE);
    size_t i = 0;
    for (i=0; i<z->dims; i++) {
	if (z->paf[i]) {
	    zonotope_aff_check_free(pr, z->paf[i]);
	    z->paf[i] = NULL;
        }
        elina_interval_free(z->box[i]);
    }
    free(z->paf);
    free(z->box);
    z->paf = NULL;
    z->box = NULL;
    size_t nsymcons_size = zonotope_noise_symbol_cons_get_dimension(pr, z);
    for (i=0; i<nsymcons_size; i++) {
	if (z->gamma[i]) {
	    if (z->gamma[i] != pr->ap_muu) elina_interval_free(z->gamma[i]);
	}
    }
    free(z->gamma);
    z->gamma = NULL;
    free(z->nsymcons);
    z->nsymcons = NULL;
    elina_abstract0_free(pr->manNS, z->abs);
    z->size = 0;
    z->dims = 0;
    z->intdim = 0;
    free(z);
    man->result.flag_best = true;
    man->result.flag_exact = true;
}



/* ********************************************************************** */
/* 3. Printing */
/* ********************************************************************** */
/* Print the zonotope */
void zonotope_fprint(FILE* stream,
		elina_manager_t* man,
		zonotope_t* z,
		char** name_of_dim)
{
    zonotope_internal_t* pr = zonotope_init_from_manager(man,ELINA_FUNID_FPRINT);
    size_t i = 0;
    fprintf(stream,"Zonotope\n");
    if (zonotope_is_bottom(man, z)) fprintf(stream,"bottom\n");
    else if (zonotope_is_top(man, z)) fprintf(stream,"top\n");
    else {
	for (i=0; i<z->dims; i++) {
	    if (z->paf[i]) {
	    if (name_of_dim) {
		fprintf(stream, "%s", name_of_dim[i]);
	    } else {
		fprintf(stream, "(%zu)", i);
	    }
	    fprintf(stream, " := ");
	    zonotope_aff_fprint(pr, stream, z->paf[i]);
            elina_interval_fprint(stdout, z->box[i]);
            fprintf(stream,"\n");
	} else {
	    fprintf(stream, "[[NULL]]\n");
	}
	}
    }
    size_t size = zonotope_noise_symbol_cons_get_dimension(pr, z);
    char** name_of_ns = (char**)malloc(size*sizeof(char*));
    elina_dim_t dim = 0;
    for (i=0; i<size; i++) {
	name_of_ns[i] = (char*)malloc(10*sizeof(char));
	pr->epsilon[z->nsymcons[i]]->type == IN ? sprintf(name_of_ns[i], "eps%d", z->nsymcons[i]) : sprintf(name_of_ns[i], "eta%d", z->nsymcons[i]);
    }
    elina_abstract0_fprint(stream, pr->manNS, z->abs, name_of_ns);
    for (i=0; i<size; i++) free(name_of_ns[i]);
    free(name_of_ns);
    name_of_ns = NULL;
    fprintf(stream,"__________\n");
    fflush(stream);
    man->result.flag_best = true;
    man->result.flag_exact = true;
}
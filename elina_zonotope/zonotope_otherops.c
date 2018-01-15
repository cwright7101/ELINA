/*
 *
 *  This source file is part of ELINA (ETH LIbrary for Numerical Analysis).
 *  ELINA is Copyright © 2017 Department of Computer Science, ETH Zurich
 *  This software is distributed under GNU Lesser General Public License
 * Version 3.0. For more information, see the ELINA project website at:
 *  http://elina.ethz.ch
 *
 *  THE SOFTWARE IS PROVIDED "AS-IS" WITHOUT ANY WARRANTY OF ANY KIND, EITHER
 *  EXPRESS, IMPLIED OR STATUTORY, INCLUDING BUT NOT LIMITED TO ANY WARRANTY
 *  THAT THE SOFTWARE WILL CONFORM TO SPECIFICATIONS OR BE ERROR-FREE AND ANY
 *  IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
 *  TITLE, OR NON-INFRINGEMENT.  IN NO EVENT SHALL ETH ZURICH BE LIABLE FOR ANY
 *  DAMAGES, INCLUDING BUT NOT LIMITED TO DIRECT, INDIRECT,
 *  SPECIAL OR CONSEQUENTIAL DAMAGES, ARISING OUT OF, RESULTING FROM, OR IN
 *  ANY WAY CONNECTED WITH THIS SOFTWARE (WHETHER OR NOT BASED UPON WARRANTY,
 *  CONTRACT, TORT OR OTHERWISE).
 *
 */

#include "zonotope_otherops.h"

zonotope_t* zonotope_forget_array(elina_manager_t* man,
		bool destructive, zonotope_t* z,
		elina_dim_t* tdim, size_t size,
		bool project)
{
    zonotope_internal_t* pr = zonotope_init_from_manager(man, ELINA_FUNID_FORGET_ARRAY);
    zonotope_t* res;
    size_t i;

    man->result.flag_best = true;
    man->result.flag_exact = true;

    res = destructive ? z : zonotope_copy(man,z);
    if (project){
	for (i=0;i<size;i++){
	    zonotope_aff_check_free(pr, res->paf[tdim[i]]);
	    res->paf[tdim[i]] = zonotope_aff_alloc_init(pr);
	    res->paf[tdim[i]]->pby++;
	}
    }
    else {
	for (i=0;i<size;i++){
	    zonotope_aff_check_free(pr, res->paf[tdim[i]]);
	    res->paf[tdim[i]] = pr->top;
	    res->paf[tdim[i]]->pby++;
	}
    }
    return res;
    //not_implemented();
}


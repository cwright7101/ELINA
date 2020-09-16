#ifdef GUROBI
#include <stdlib.h>
#include <stdio.h>

#include "gurobi_c.h"
#endif

#include "compute_bounds.h"
#include "math.h"

expr_t * replace_input_poly_cons_in_lexpr(fppoly_internal_t *pr, expr_t * expr, fppoly_t * fp){
	size_t dims = expr->size;
	size_t i,k;
	double tmp1, tmp2;
	expr_t * res;
	if(expr->type==DENSE){
		k = 0;
	}
	else{
		k = expr->dim[0];		
	}
	expr_t * mul_expr = NULL;
			
	if(expr->sup_coeff[0] <0){
		mul_expr = fp->input_uexpr[k];
	}
	else if(expr->inf_coeff[0] < 0){
		mul_expr = fp->input_lexpr[k];
	}
		
	if(mul_expr!=NULL){
		if(mul_expr->size==0){
			res = multiply_cst_expr(pr,mul_expr,expr->inf_coeff[0],expr->sup_coeff[0]);
		}
		else{
			res = multiply_expr(pr,mul_expr,expr->inf_coeff[0],expr->sup_coeff[0]);
		}
	}
		
	else{
		elina_double_interval_mul_cst_coeff(pr,&tmp1,&tmp2,expr->inf_coeff[0],expr->sup_coeff[0],fp->input_inf[k],fp->input_sup[k]);
		res = create_cst_expr(tmp1, -tmp1);
	}
	for(i=1; i < dims; i++){
		if(expr->type==DENSE){
			k = i;
		}
		else{
			k = expr->dim[i];
		}
			
		expr_t * mul_expr = NULL;
		expr_t * sum_expr = NULL;
		if(expr->sup_coeff[i] <0){
			mul_expr = fp->input_uexpr[k];
		}
		else if(expr->inf_coeff[i] <0){
			mul_expr = fp->input_lexpr[k];
		}
			
		if(mul_expr!=NULL){
			if(mul_expr->size==0){
				sum_expr = multiply_cst_expr(pr,mul_expr, expr->inf_coeff[i],expr->sup_coeff[i]);
				add_cst_expr(pr,res,sum_expr);
			}	
			else if(expr->inf_coeff[i]!=0 && expr->sup_coeff[i]!=0){
				sum_expr = multiply_expr(pr,mul_expr, expr->inf_coeff[i],expr->sup_coeff[i]);
				add_expr(pr,res,sum_expr);
			}
				//free_expr(mul_expr);
			if(sum_expr!=NULL){
				free_expr(sum_expr);
			}
		}
		else{
			elina_double_interval_mul_cst_coeff(pr,&tmp1,&tmp2,expr->inf_coeff[i],expr->sup_coeff[i],fp->input_inf[k],fp->input_sup[k]);
			res->inf_cst = res->inf_cst + tmp1;
			res->sup_cst = res->sup_cst - tmp1;
		}
	}
		
	res->inf_cst = res->inf_cst + expr->inf_cst; 
	res->sup_cst = res->sup_cst + expr->sup_cst; 
	return res;
}


expr_t * replace_input_poly_cons_in_uexpr(fppoly_internal_t *pr, expr_t * expr, fppoly_t * fp){
	size_t dims = expr->size;
	size_t i,k;
	double tmp1, tmp2;
	expr_t * res;
	if(expr->type==DENSE){
		k = 0;
	}
	else{
		k = expr->dim[0];		
	}
	expr_t * mul_expr = NULL;
			
	if(expr->sup_coeff[0] <0){
		mul_expr = fp->input_lexpr[k];
	}
	else if(expr->inf_coeff[0] < 0){
		mul_expr = fp->input_uexpr[k];
	}
		
	if(mul_expr!=NULL){
		if(mul_expr->size==0){
			res = multiply_cst_expr(pr,mul_expr,expr->inf_coeff[0],expr->sup_coeff[0]);
		}
		else{
			res = multiply_expr(pr,mul_expr,expr->inf_coeff[0],expr->sup_coeff[0]);
		}
	}
	else{
		elina_double_interval_mul_cst_coeff(pr,&tmp1,&tmp2,expr->inf_coeff[0],expr->sup_coeff[0],fp->input_inf[k],fp->input_sup[k]);
		res = create_cst_expr(-tmp2, tmp2);
	}
                //printf("finish\n");
		//fflush(stdout);
	for(i=1; i < dims; i++){
		if(expr->type==DENSE){
			k = i;
		}
		else{
			k = expr->dim[i];
		}
		expr_t * mul_expr = NULL;
		expr_t * sum_expr = NULL;
		if(expr->sup_coeff[i] <0){
			mul_expr = fp->input_lexpr[k];
		}
		else if(expr->inf_coeff[i] <0){
			mul_expr = fp->input_uexpr[k];
		}
			
		if(mul_expr!=NULL){
			if(mul_expr->size==0){
				sum_expr = multiply_cst_expr(pr,mul_expr, expr->inf_coeff[i],expr->sup_coeff[i]);
				add_cst_expr(pr,res,sum_expr);
			}	
			else if(expr->inf_coeff[i]!=0 && expr->sup_coeff[i]!=0){
				sum_expr = multiply_expr(pr,mul_expr, expr->inf_coeff[i],expr->sup_coeff[i]);
				add_expr(pr,res,sum_expr);
			}
				//free_expr(mul_expr);
			if(sum_expr!=NULL){
				free_expr(sum_expr);
			}
		}
		else{
			elina_double_interval_mul_cst_coeff(pr,&tmp1,&tmp2,expr->inf_coeff[i],expr->sup_coeff[i],fp->input_inf[k],fp->input_sup[k]);
			res->inf_cst = res->inf_cst - tmp2;
			res->sup_cst = res->sup_cst + tmp2;
		}
	}
	res->inf_cst = res->inf_cst + expr->inf_cst; 
	res->sup_cst = res->sup_cst + expr->sup_cst; 
	return res;
}


double compute_lb_from_expr(fppoly_internal_t *pr, expr_t * expr, fppoly_t * fp, int layerno){
  size_t k;
  double tmp1, tmp2;
  // printf("start\n");
  // fflush(stdout);
  if ((fp->input_lexpr != NULL) && (fp->input_uexpr != NULL) && layerno == -1) {
    expr = replace_input_poly_cons_in_lexpr(pr, expr, fp);
	}
        //expr_print(expr);
	//fflush(stdout);
	size_t dims = expr->size;
	double res_inf = expr->inf_cst;
	if(expr->inf_coeff==NULL || expr->sup_coeff==NULL){
		return res_inf;
	}

        double *expr_coeffs;
        const size_t num_pixels = fp->num_pixels;
        bool has_spatial_constraints =
            (fp->spatial_constraints_size > 0) && (layerno == -1);

        if (has_spatial_constraints) {
          expr_coeffs = malloc(dims * sizeof(double));
        }

        for (size_t i = 0; i < dims; i++) {
          // if(expr->inf_coeff[i]<0){
          if (expr->type == DENSE) {
            k = i;
          } else {
            k = expr->dim[i];
          }
          if (layerno == -1) {
            elina_double_interval_mul(&tmp1, &tmp2, expr->inf_coeff[i],
                                      expr->sup_coeff[i], fp->input_inf[k],
                                      fp->input_sup[k]);

            if (has_spatial_constraints) {
              expr_coeffs[i] = expr->inf_coeff[i];
            }
          } else {
            elina_double_interval_mul(&tmp1, &tmp2, expr->inf_coeff[i],
                                      expr->sup_coeff[i],
                                      fp->layers[layerno]->neurons[k]->lb,
                                      fp->layers[layerno]->neurons[k]->ub);
          }
          // printf("tmp1: %g\n",tmp1);
          res_inf = res_inf + tmp1;
        }
//	printf("inf: %g\n",-res_inf);
//	fflush(stdout);
        if(fp->input_lexpr!=NULL && fp->input_uexpr!=NULL && layerno==-1){
		free_expr(expr);
	}
        //printf("finish\n");
        //fflush(stdout);

        if (has_spatial_constraints) {
          size_t index, neighbor, sparse_index, sparse_neighbor;
          double res_inf_spatial = expr->inf_cst;

          for (size_t i = 0; i < fp->spatial_constraints_size; ++i) {
            index = fp->spatial_indices[i];
            neighbor = fp->spatial_neighbors[i];

            if (expr->type == DENSE) {
              sparse_index = index;
              sparse_neighbor = neighbor;
            } else {
              sparse_index = sparse_neighbor = num_pixels;

              for (size_t j = 0; j < dims; ++j) {
                if (expr->dim[j] == index) {
                  sparse_index = j;
                }
                if (expr->dim[j] == neighbor) {
                  sparse_neighbor = j;
                }
              }

              if (sparse_index == num_pixels || sparse_neighbor == num_pixels) {
                continue;
              }
            }

            // expr_coeffs are inverted as they contain lower bounds
            if (expr_coeffs[sparse_index] != 0 &&
                expr_coeffs[sparse_neighbor] != 0) {
              double factor_index = expr_coeffs[sparse_index];
              double rest_neighbor =
                  expr_coeffs[sparse_index] + expr_coeffs[sparse_neighbor];
              elina_double_interval_mul(&tmp1, &tmp2, rest_neighbor,
                                        -rest_neighbor, fp->input_inf[neighbor],
                                        fp->input_sup[neighbor]);
              double lb_index =
                  tmp1 + factor_index * fp->spatial_lower_bounds[i];

              double factor_neighbor = expr_coeffs[sparse_neighbor];
              double rest_index =
                  expr_coeffs[sparse_index] + expr_coeffs[sparse_neighbor];
              elina_double_interval_mul(&tmp1, &tmp2, rest_index, -rest_index,
                                        fp->input_inf[index],
                                        fp->input_sup[index]);
              double lb_neighbor =
                  tmp1 - factor_neighbor * fp->spatial_upper_bounds[i];

              elina_double_interval_mul(&tmp1, &tmp2, expr_coeffs[sparse_index],
                                        -expr_coeffs[sparse_index],
                                        fp->input_inf[index],
                                        fp->input_sup[index]);
              double lb_original = tmp1;
              elina_double_interval_mul(
                  &tmp1, &tmp2, expr_coeffs[sparse_neighbor],
                  -expr_coeffs[sparse_neighbor], fp->input_inf[neighbor],
                  fp->input_sup[neighbor]);
              lb_original += tmp1;

              // flow constraints don't improve the bound
              if (lb_original < fmin(lb_index, lb_neighbor)) {
                continue;
              }

              if (lb_index < lb_neighbor) {
                expr_coeffs[sparse_index] = 0;
                expr_coeffs[sparse_neighbor] = rest_neighbor;
                res_inf_spatial += factor_index * fp->spatial_lower_bounds[i];
              } else {
                expr_coeffs[sparse_index] = rest_index;
                expr_coeffs[sparse_neighbor] = 0;
                res_inf_spatial -=
                    factor_neighbor * fp->spatial_upper_bounds[i];
              }
            }
          }

          // compute lower bounds for variables with nonzero coefficients
          for (size_t i = 0; i < dims; ++i) {
            k = expr->type == DENSE ? i : expr->dim[i];

            if (expr_coeffs[i] != 0) {
              elina_double_interval_mul(&tmp1, &tmp2, expr_coeffs[i],
                                        -expr_coeffs[i], fp->input_inf[k],
                                        fp->input_sup[k]);
              res_inf_spatial += tmp1;
            }
          }

          free(expr_coeffs);

          return res_inf_spatial;
        }

        return res_inf;
}

double compute_ub_from_expr(fppoly_internal_t *pr, expr_t *expr, fppoly_t *fp,
                            int layerno) {
  size_t k;
  double tmp1, tmp2;

  if ((fp->input_lexpr != NULL) && (fp->input_uexpr != NULL) && layerno == -1) {
    expr = replace_input_poly_cons_in_uexpr(pr, expr, fp);
  }

  size_t dims = expr->size;
  double res_sup = expr->sup_cst;
  if (expr->inf_coeff == NULL || expr->sup_coeff == NULL) {
    return res_sup;
  }

  double *expr_coeffs;
  const size_t num_pixels = fp->num_pixels;
  bool has_spatial_constraints =
      (fp->spatial_constraints_size > 0) && (layerno == -1);

  if (has_spatial_constraints) {
    expr_coeffs = malloc(dims * sizeof(double));
  }

  for (size_t i = 0; i < dims; i++) {
    // if(expr->inf_coeff[i]<0){
    if (expr->type == DENSE) {
      k = i;
    } else {
      k = expr->dim[i];
    }
    if (layerno == -1) {
      elina_double_interval_mul(&tmp1, &tmp2, expr->inf_coeff[i],
                                expr->sup_coeff[i], fp->input_inf[k],
                                fp->input_sup[k]);

      if (has_spatial_constraints) {
        expr_coeffs[i] = expr->sup_coeff[i];
      }
    } else {
      elina_double_interval_mul(&tmp1, &tmp2, expr->inf_coeff[i],
                                expr->sup_coeff[i],
                                fp->layers[layerno]->neurons[k]->lb,
                                fp->layers[layerno]->neurons[k]->ub);
    }
    res_sup = res_sup + tmp2;
  }
  // printf("sup: %g\n",res_sup);
  // fflush(stdout);
  if (fp->input_lexpr != NULL && fp->input_uexpr != NULL && layerno == -1) {
    free_expr(expr);
  }

  if (has_spatial_constraints) {
    size_t index, neighbor, sparse_index, sparse_neighbor;
    double res_sup_spatial = expr->sup_cst;

    for (size_t i = 0; i < fp->spatial_constraints_size; ++i) {
      index = fp->spatial_indices[i];
      neighbor = fp->spatial_neighbors[i];

      if (expr->type == DENSE) {
        sparse_index = index;
        sparse_neighbor = neighbor;
      } else {
        sparse_index = sparse_neighbor = num_pixels;

        for (size_t j = 0; j < dims; ++j) {
          if (expr->dim[j] == index) {
            sparse_index = j;
          }
          if (expr->dim[j] == neighbor) {
            sparse_neighbor = j;
          }
        }

        if (sparse_index == num_pixels || sparse_neighbor == num_pixels) {
          continue;
        }
      }

      if (expr_coeffs[sparse_index] != 0 && expr_coeffs[sparse_neighbor] != 0) {
        double factor_index = expr_coeffs[sparse_index];
        double rest_neighbor =
            expr_coeffs[sparse_index] + expr_coeffs[sparse_neighbor];
        elina_double_interval_mul(&tmp1, &tmp2, -rest_neighbor, rest_neighbor,
                                  fp->input_inf[neighbor],
                                  fp->input_sup[neighbor]);
        double ub_index = tmp2 + factor_index * fp->spatial_upper_bounds[i];

        double factor_neighbor = expr_coeffs[sparse_neighbor];
        double rest_index =
            expr_coeffs[sparse_index] + expr_coeffs[sparse_neighbor];
        elina_double_interval_mul(&tmp1, &tmp2, -rest_index, rest_index,
                                  fp->input_inf[index], fp->input_sup[index]);
        double ub_neighbor =
            tmp2 - factor_neighbor * fp->spatial_lower_bounds[i];

        elina_double_interval_mul(&tmp1, &tmp2, -expr_coeffs[sparse_index],
                                  expr_coeffs[sparse_index],
                                  fp->input_inf[index], fp->input_sup[index]);
        double ub_original = tmp2;
        elina_double_interval_mul(&tmp1, &tmp2, -expr_coeffs[sparse_neighbor],
                                  expr_coeffs[sparse_neighbor],
                                  fp->input_inf[neighbor],
                                  fp->input_sup[neighbor]);
        ub_original += tmp2;

        // flow constraints don't improve the bound
        if (ub_original < fmin(ub_index, ub_neighbor)) {
          continue;
        }

        if (ub_index < ub_neighbor) {
          expr_coeffs[sparse_index] = 0;
          expr_coeffs[sparse_neighbor] = rest_neighbor;
          res_sup_spatial += factor_index * fp->spatial_upper_bounds[i];
        } else {
          expr_coeffs[sparse_index] = rest_index;
          expr_coeffs[sparse_neighbor] = 0;
          res_sup_spatial -= factor_neighbor * fp->spatial_lower_bounds[i];
        }
      }
    }

    // compute upper bounds for variables with nonzero coefficients
    for (size_t i = 0; i < dims; ++i) {
      k = expr->type == DENSE ? i : expr->dim[i];

      if (expr_coeffs[i] != 0) {
        elina_double_interval_mul(&tmp1, &tmp2, -expr_coeffs[i], expr_coeffs[i],
                                  fp->input_inf[k], fp->input_sup[k]);
        res_sup_spatial += tmp2;
      }
    }

    free(expr_coeffs);

    return res_sup_spatial;
  }

  return res_sup;
}

double get_lb_using_predecessor_layer(fppoly_internal_t * pr,fppoly_t *fp, expr_t **lexpr_ptr, int k){
	expr_t * tmp_l;
	neuron_t ** aux_neurons = fp->layers[k]->neurons;
	expr_t *lexpr = *lexpr_ptr;
	double res = INFINITY;
	res = compute_lb_from_expr(pr,lexpr,fp,k);
	tmp_l = lexpr;
	*lexpr_ptr = lexpr_replace_bounds(pr,lexpr,aux_neurons, fp->layers[k]->is_activation);
	free_expr(tmp_l);
	return res;
}


double get_ub_using_predecessor_layer(fppoly_internal_t * pr,fppoly_t *fp, expr_t **uexpr_ptr, int k){
	expr_t * tmp_u;
	neuron_t ** aux_neurons = fp->layers[k]->neurons;
	expr_t *uexpr = *uexpr_ptr;
	double res = INFINITY;
	tmp_u = uexpr;
	res = compute_ub_from_expr(pr,uexpr,fp,k);
	*uexpr_ptr = uexpr_replace_bounds(pr,uexpr,aux_neurons, fp->layers[k]->is_activation);
	free_expr(tmp_u);
	return res;
}

double get_lb_using_previous_layers(elina_manager_t *man, fppoly_t *fp, expr_t *expr, size_t layerno){
	size_t i;
	int k;
	//size_t numlayers = fp->numlayers;
	expr_t * lexpr = copy_expr(expr);
        fppoly_internal_t * pr = fppoly_init_from_manager(man,ELINA_FUNID_ASSIGN_LINEXPR_ARRAY);
	if(fp->numlayers==layerno){
		
		k = layerno-1;
        } else if (fp->layers[layerno]->num_predecessors == 2) {
          k = layerno;
        } else {
          k = fp->layers[layerno]->predecessors[0] - 1;
        }
        double res = INFINITY;
	while(k >=0){

          if (fp->layers[k]->num_predecessors == 2) {
            expr_t *lexpr_copy = copy_expr(lexpr);
            lexpr_copy->inf_cst = 0;
            lexpr_copy->sup_cst = 0;
            size_t predecessor1 = fp->layers[k]->predecessors[0] - 1;
            size_t predecessor2 = fp->layers[k]->predecessors[1] - 1;

            char *predecessor_map = (char *)calloc(k, sizeof(char));
            // Assume no nested residual layers
            int iter = fp->layers[predecessor1]->predecessors[0] - 1;
            while (iter >= 0) {
              predecessor_map[iter] = 1;
              iter = fp->layers[iter]->predecessors[0] - 1;
            }
            iter = fp->layers[predecessor2]->predecessors[0] - 1;
            int common_predecessor = 0;
            while (iter >= 0) {
              if (predecessor_map[iter] == 1) {
                common_predecessor = iter;
                break;
              }
              iter = fp->layers[iter]->predecessors[0] - 1;
            }

            iter = predecessor1;
            while (iter != common_predecessor) {
              get_lb_using_predecessor_layer(pr, fp, &lexpr, iter);
              iter = fp->layers[iter]->predecessors[0] - 1;
            }
            iter = predecessor2;
            while (iter != common_predecessor) {
              get_lb_using_predecessor_layer(pr, fp, &lexpr_copy, iter);
              iter = fp->layers[iter]->predecessors[0] - 1;
            }
            free(predecessor_map);
            add_expr(pr, lexpr, lexpr_copy);

            free_expr(lexpr_copy);

            // Assume at least one non-residual layer between two residual
            // layers
            k = common_predecessor;

            continue;
          } else {

            res = fmin(res, get_lb_using_predecessor_layer(pr, fp, &lexpr, k));
            k = fp->layers[k]->predecessors[0] - 1;
          }
        }
		
	res = fmin(res,compute_lb_from_expr(pr,lexpr,fp,-1)); 
        free_expr(lexpr);
	return res;
	
}


double get_ub_using_previous_layers(elina_manager_t *man, fppoly_t *fp, expr_t *expr, size_t layerno){
	size_t i;
	int k;
	//size_t numlayers = fp->numlayers;
	expr_t * uexpr = copy_expr(expr);
        fppoly_internal_t * pr = fppoly_init_from_manager(man,ELINA_FUNID_ASSIGN_LINEXPR_ARRAY);
        
	if(fp->numlayers==layerno){
		k = layerno-1;
        } else if (fp->layers[layerno]->num_predecessors == 2) {
          k = layerno;
        } else {
          k = fp->layers[layerno]->predecessors[0] - 1;
        }
        double res =INFINITY;
	while(k >=0){
          if (fp->layers[k]->num_predecessors == 2) {
            expr_t *uexpr_copy = copy_expr(uexpr);
            uexpr_copy->inf_cst = 0;
            uexpr_copy->sup_cst = 0;
            size_t predecessor1 = fp->layers[k]->predecessors[0] - 1;
            size_t predecessor2 = fp->layers[k]->predecessors[1] - 1;

            char *predecessor_map = (char *)calloc(k, sizeof(char));
            // Assume no nested residual layers
            int iter = fp->layers[predecessor1]->predecessors[0] - 1;
            while (iter >= 0) {
              predecessor_map[iter] = 1;
              iter = fp->layers[iter]->predecessors[0] - 1;
            }
            iter = fp->layers[predecessor2]->predecessors[0] - 1;
            int common_predecessor = 0;
            while (iter >= 0) {
              if (predecessor_map[iter] == 1) {
                common_predecessor = iter;
                break;
              }
              iter = fp->layers[iter]->predecessors[0] - 1;
            }

            iter = predecessor1;
            while (iter != common_predecessor) {
              get_ub_using_predecessor_layer(pr, fp, &uexpr, iter);
              iter = fp->layers[iter]->predecessors[0] - 1;
            }
            iter = predecessor2;
            while (iter != common_predecessor) {
              get_ub_using_predecessor_layer(pr, fp, &uexpr_copy, iter);
              iter = fp->layers[iter]->predecessors[0] - 1;
            }
            free(predecessor_map);
            add_expr(pr, uexpr, uexpr_copy);

            free_expr(uexpr_copy);

            // Assume at least one non-residual layer between two residual
            // layers
            k = common_predecessor;

            continue;
          } else {

            res = fmin(res, get_ub_using_predecessor_layer(pr, fp, &uexpr, k));
            k = fp->layers[k]->predecessors[0] - 1;
          }
        }
		
	res = fmin(res,compute_ub_from_expr(pr,uexpr,fp,-1)); 
        free_expr(uexpr);
	return res;
	
}

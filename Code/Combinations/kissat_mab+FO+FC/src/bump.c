#include "bump.h"
#include "internal.h"
#include "logging.h"
#include "print.h"
#include "rank.h"
#include "sort.h"
#include "decide.h"
#include "trail.h"
#include "reduce.h"

static inline unsigned
rank_of_idxrank (idxrank ir)
{
  return ir.rank;
}

static inline bool
smaller_idxrank (idxrank ir, idxrank jr)
{
  return ir.rank < jr.rank;
}

#define RADIX_SORT_BUMP_LIMIT 800
#define RADIX_SORT_BUMP_LENGTH 8

static void
sort_bump (kissat * solver)
{
  const size_t size = SIZE_STACK (solver->analyzed);
  if (size < RADIX_SORT_BUMP_LIMIT)
    {
      LOG ("quick sorting %zu analyzed variables", size);
      SORT_STACK (idxrank, solver->bump, smaller_idxrank);
    }
  else
    {
      LOG ("radix sorting %zu analyzed variables", size);
      RADIX_STACK (RADIX_SORT_BUMP_LENGTH, idxrank,
		   unsigned, solver->bump, rank_of_idxrank);
    }
}

static void
rescale_scores (kissat * solver, heap * scores)
{
  INC (rescaled);
  const double max_score = kissat_max_score_on_heap (scores);
  kissat_phase (solver, "rescale", GET (rescaled),
		"maximum score %g increment %g", max_score, solver->scinc);
  const double rescale = MAX (max_score, solver->scinc);
  assert (rescale > 0);
  const double factor = 1.0 / rescale;
  kissat_rescale_heap (solver, scores, factor);
  solver->scinc *= factor;
  kissat_phase (solver, "rescale",
		GET (rescaled), "rescaled by factor %g", factor);
}

static void
bump_score_increment (kissat * solver, heap * scores)
{
  const double old_scinc = solver->scinc;
  const double decay = GET_OPTION (decay) * 1e-3;
  assert (0 <= decay), assert (decay <= 0.5);
  const double factor = 1.0 / (1.0 - decay);
  const double new_scinc = old_scinc * factor;
  LOG ("new score increment %g = %g * %g", new_scinc, factor, old_scinc);
  solver->scinc = new_scinc;
  if (new_scinc > MAX_SCORE)
    rescale_scores (solver, scores);
}

static inline void
bump_variable_score (kissat * solver, heap * scores, unsigned idx)
{
  const double old_score = kissat_get_heap_score (scores, idx);
  const double new_score = old_score + solver->scinc;
  LOG ("new score[%u] = %g = %g + %g",
       idx, new_score, old_score, solver->scinc);
  kissat_update_heap (solver, scores, idx, new_score);
  if (new_score > MAX_SCORE)
    rescale_scores (solver, scores);
}

static bool
mark_learn(kissat *solver, reference start_ref)
{
  int h = 0;
  assert(start_ref != INVALID_REF);
  assert(start_ref <= SIZE_STACK(solver->arena));
  const word *arena = BEGIN_STACK(solver->arena);
  clause *start = (clause *)(arena + start_ref);
  const clause *end = (clause *)END_STACK(solver->arena);
  assert(start < end);
  while (start != end && (!start->redundant || start->keep))
    start = kissat_next_clause(start);
  if (start == end)
  {
    solver->first_reducible = INVALID_REF;
    LOG("no reducible clause candidate left");
    return false;
  }
  const reference redundant = (word *)start - arena;
  solver->first_reducible = redundant;
  const unsigned tier2 = GET_OPTION(tier2);
#ifndef QUIET
  size_t flushed_hyper_ternary_clauses = 0;
  size_t used_hyper_ternary_clauses = 0;
#endif
int x1 = 0, x2 = 0, x3 = 0;
  for (clause *c = start; c != end; c = kissat_next_clause(c))
  {
    if (!c->redundant)
      continue;
    if (c->garbage)
    {++x1; continue;}
    if (c->reason)
    {++x2; continue;}
    if (c->hyper)
    {
      assert(c->size == 3);
      if (c->used)
      {
#ifndef QUIET
        used_hyper_ternary_clauses++;
#endif
        c->used = false;
      }
      else
      {
#ifndef QUIET
        flushed_hyper_ternary_clauses++;
#endif
        kissat_mark_clause_as_garbage(solver, c);
      }
      continue;
    }
    // if (c->keep) {
    //   ++x3; continue;
    // }
    
    kissat_mark_clause_as_garbage(solver, c);
    ++h;
  }
  printf("delete %d( %d, %d %d)\n", h, x1, x2, x3);
}

static bool
compacting2(kissat *solver)
{
  if (!GET_OPTION(compact))
    return false;
  unsigned inactive = solver->vars - solver->active;
  unsigned limit = GET_OPTION(compactlim) / 1e2 * solver->vars;
  bool compact = (inactive > limit);
  LOG("%u inactive variables %.0f%% <= limit %u %.0f%%",
      inactive, kissat_percent(inactive, solver->vars),
      limit, kissat_percent(limit, solver->vars));
  return compact;
}

void kissat_shuffle_score(kissat * solver) {
  bool compact = compacting2(solver);
  reference start = compact ? 0 : solver->first_reducible;
  if (start != INVALID_REF)
  {
    if (kissat_flush_and_mark_reason_clauses(solver, start))
    {
      mark_learn(solver, start);
      kissat_sparse_collect(solver, compact, start);
    }
  }

  heap *scores = &solver->scores;
  heap *scores_chb = &solver->scores_chb;
  flags *flags = solver->flags;
  int *id = (int *)malloc(sizeof(int) * (VARS));
  for (int i = 0; i < VARS; i++) id[i] = i;
  for (int i = 0; i < VARS; i++)
  {
    int j = (rand() % VARS);
    int x = id[i];
    id[i] = id[j];
    id[j] = x;
  }
  for (int i = 0; i < VARS; i++) {
    int idx = id[i];
    if (flags[idx].active) {
      double new_score = 1.0 * i / VARS;
      kissat_update_heap (solver, scores, idx, new_score);
      kissat_update_heap (solver, scores_chb, idx, new_score);
    }
  }
  for (int i = 0; i < VARS; i++) {
    int idx = id[i];
    if (flags[idx].active)
      kissat_move_to_front(solver, idx);
  }
  solver->scinc = 1.0;
  testlast(solver);
  free(id);
}

void kissat_bump_one(kissat * solver, int eidx) {
  if (eidx == -1) return;
  assert(eidx >= 0);
  assert(eidx < VARS);
  assert(!solver->probing);
  //printf("bump %d\n", idx);
  flags *flags = solver->flags;
  import *import = &PEEK_STACK (solver->import, eidx);
  assert(import->imported);
  if (import->eliminated) return;
  int idx = (import->lit) >> 1;
  //vsids 
  if (solver->stable && solver->heuristic == 0 && flags[idx].active && kissat_heap_contains (&solver->scores, idx))
    kissat_update_heap(solver, &solver->scores, idx, 1e100);

  //vmtf
  links *links = solver->links;
  if (!solver->stable && flags[idx].active && !VALUE(LIT(idx)) && links[idx].prev != DISCONNECT && links[idx].next != DISCONNECT) {
    // queue *queue = &solver->queue;
    // printf("c stamp %d\n", queue->stamp);   
    kissat_move_to_front(solver, idx);
  }

  //chb
  if (solver->stable && solver->heuristic == 1 && flags[idx].active && kissat_heap_contains (&solver->scores_chb, idx))
    kissat_update_heap (solver, &solver->scores_chb, idx, 1.0);
}

static void
bump_analyzed_variable_scores (kissat * solver)
{
  heap *scores = &solver->scores;
  flags *flags = solver->flags;

  for (all_stack (unsigned, idx, solver->analyzed))
    if (flags[idx].active)
        bump_variable_score (solver, scores, idx);

  bump_score_increment (solver, scores);
}

static void
move_analyzed_variables_to_front_of_queue (kissat * solver)
{
  assert (EMPTY_STACK (solver->bump));
  const links *links = solver->links;
  for (all_stack (unsigned, idx, solver->analyzed))
    {
// *INDENT-OFF*
    const idxrank idxrank = { .idx = idx, .rank = links[idx].stamp };
// *INDENT-ON*
      PUSH_STACK (solver->bump, idxrank);
    }

  sort_bump (solver);

  flags *flags = solver->flags;
  unsigned idx;

  for (all_stack (idxrank, idxrank, solver->bump))
    if (flags[idx = idxrank.idx].active)
      kissat_move_to_front (solver, idx);

  CLEAR_STACK (solver->bump);
}

void
kissat_bump_variables (kissat * solver)
{
  START (bump);
  assert (!solver->probing);
  if (solver->stable)
    bump_analyzed_variable_scores (solver);
  else
    move_analyzed_variables_to_front_of_queue (solver);
  STOP (bump);
}

// CHB

void kissat_bump_chb(kissat * solver, unsigned v, double multiplier) {
    int64_t age = solver->statistics.conflicts - solver->conflicted_chb[v] + 1;
    double reward_chb = multiplier / age;
    double old_score = kissat_get_heap_score (&solver->scores_chb, v);
    double new_score = solver->step_chb * reward_chb + (1 - solver->step_chb) * old_score;
    LOG ("new score[%u] = %g vs %g",
       v, new_score, old_score);
    kissat_update_heap (solver, &solver->scores_chb, v, new_score);
}

void kissat_decay_chb(kissat * solver){
    if (solver->step_chb > solver->step_min_chb) solver->step_chb -= solver->step_dec_chb;
}

void
kissat_update_conflicted_chb (kissat * solver)
{
  flags *flags = solver->flags;

  for (all_stack (unsigned, idx, solver->analyzed))
    if (flags[idx].active)
        solver->conflicted_chb[idx]=solver->statistics.conflicts;
}

#ifndef _EVALUATOR_H_
#define _EVALUATOR_H_

#define TIME_SLICE_LENGTH 100

typedef enum Reason {
  reason_terminated,
  reason_timeslice_ended,
  reason_blocked,
} ReasonT;

typedef struct EvaluatorResult {
  unsigned int PC;
  unsigned int cpu_time;
  ReasonT reason;
} EvaluatorResultT;

typedef struct EvaluatorCode {
  EvaluatorResultT (*implementation)(unsigned int, unsigned int);
  unsigned int parameter;
} EvaluatorCodeT;


// The evaluator - pretends to run some code on a CPU
EvaluatorResultT evaluator_evaluate(EvaluatorCodeT const code, unsigned int PC);

// A CPU bound process that terminates after specified steps
EvaluatorCodeT evaluator_terminates_after(unsigned int steps);

// A CPU bound process that never terminates
extern EvaluatorCodeT const evaluator_infinite_loop;

// A process that terminates after specified steps and may block
EvaluatorCodeT evaluator_blocking_terminates_after(unsigned int steps);

#endif

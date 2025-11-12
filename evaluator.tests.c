#include "evaluator.h"

#include <assert.h>
#include <stdio.h>

void test_evaluator_infinite_loop() {
  printf("testing infinite loop\n");
  for(int i = 0, PC = 0; i != 1000; ++i) { // Can't test to infinity!
    EvaluatorResultT const result =
      evaluator_evaluate(evaluator_infinite_loop, PC);
    assert(result.reason == reason_timeslice_ended);
    assert(result.cpu_time == TIME_SLICE_LENGTH);
    PC = result.PC;
  }
}

void test_evaluator_terminates_after() {
  printf("testing terminating CPU bound process\n");
  unsigned int const steps = 20;
  EvaluatorCodeT const code = evaluator_terminates_after(steps);
  unsigned int PC = 0;
  for(int step = 1; step != steps; ++step) {
    EvaluatorResultT const result = evaluator_evaluate(code, PC);
    assert(result.reason == reason_timeslice_ended);
    assert(result.cpu_time == TIME_SLICE_LENGTH);
    PC = result.PC;
  }
  EvaluatorResultT const result = evaluator_evaluate(code, PC);
  assert(result.reason == reason_terminated);
  assert(result.cpu_time <= TIME_SLICE_LENGTH);
}

void test_evaluator_blocking() {
  printf("testing terminating process that may block\n");
  unsigned int const steps = 20;
  EvaluatorCodeT const code = evaluator_blocking_terminates_after(steps);
  unsigned int PC = 0;
  for(int step = 1; step != steps; ++step) {
    EvaluatorResultT const result = evaluator_evaluate(code, PC);
    assert(result.reason != reason_terminated);
    if(result.reason == reason_timeslice_ended) {
      assert(result.cpu_time == TIME_SLICE_LENGTH);
    } else {
      assert(result.cpu_time <= TIME_SLICE_LENGTH);
    }
    PC = result.PC;
  }
  EvaluatorResultT const result = evaluator_evaluate(code, PC);
  assert(result.reason == reason_terminated);
  assert(result.cpu_time <= TIME_SLICE_LENGTH);
}

void test_evaluator_specification_examples() {
  evaluator_evaluate(evaluator_terminates_after(5), 0);
  evaluator_evaluate(evaluator_infinite_loop, 0);
  evaluator_evaluate(evaluator_blocking_terminates_after(5), 0);
}

int main() {
  test_evaluator_infinite_loop();
  test_evaluator_terminates_after();
  test_evaluator_blocking();
  test_evaluator_specification_examples();
  return 0;
}

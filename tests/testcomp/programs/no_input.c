// No nondeterministic input at all. The suite must still carry one test case
// with zero <input> elements: emitting no test case would make the suite
// unscoreable even though the error is trivially reachable, and Test-Comp
// tasks with no input do exist.
extern void reach_error(void);

int main(void) {
  reach_error();
  return 0;
}

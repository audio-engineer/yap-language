#if !defined(BENCHMARK_H) && defined(__CC65__)
#define BENCHMARK_H

/*
Trick to hide __near__ from ClangTidy: Define __near__ as an empty string when
ClangTidy runs.
*/
#ifdef NNEAR
// NOLINTBEGIN(bugprone-reserved-identifier)
#define __near__
#endif

void __near__ StartTimerA();
void __near__ StopTimerA();

// NOLINTEND(bugprone-reserved-identifier)

#endif  // BENCHMARK_H

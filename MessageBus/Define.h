#ifndef __CPP17
#if __cplusplus > 201402L
#       define __CPP17 (1)
#else
#       define __CPP17 (0)
#endif
#endif

#ifndef __CPP14
#if __cplusplus > 201103L
#       define __CPP14 (1)
#else
#       define __CPP14 (0)
#endif
#endif

#ifndef __CPP11
#if __cplusplus > 199711L
#       define __CPP11 (1)
#else
#       define __CPP11 (0)
#endif
#endif
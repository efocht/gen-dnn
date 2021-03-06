#define _GNU_SOURCE
#include "cloexec.h"
#include "common.h"
#include <sched.h>
//#include "util.h"
#include <sched.h>
#include <unistd.h>
#include <syscall.h>

//#include "asm/bug.h"
#include <stdio.h>
#define __WARN_printf(arg...)   do { fprintf(stderr, arg); } while (0)
//
#define WARN(condition, format...) ({           \
        int __ret_warn_on = !!(condition);      \
        if (unlikely(__ret_warn_on))            \
                __WARN_printf(format);          \
        unlikely(__ret_warn_on);                \
})
//
#define WARN_ONCE(condition, format...) ({      \
        static int __warned;                    \
        int __ret_warn_once = !!(condition);    \
                                                \
        if (unlikely(__ret_warn_once))          \
                if (WARN(!__warned, format))    \
                        __warned = 1;           \
        unlikely(__ret_warn_once);              \
})
// ... bug.h

//#include "../perf.h" --> common.h

static unsigned long flag = PERF_FLAG_FD_CLOEXEC;

#ifdef __GLIBC_PREREQ
#if !__GLIBC_PREREQ(2, 6)
int __weak sched_getcpu(void)
{
        errno = ENOSYS;
        return -1;
}
#endif
#endif

static int perf_flag_probe(void)
{
        /* use 'safest' configuration as used in perf_evsel__fallback() */
        struct perf_event_attr attr = {
                .type = PERF_TYPE_SOFTWARE,
                .config = PERF_COUNT_SW_CPU_CLOCK,
                .exclude_kernel = 1,
        };
        int fd;
        int err;
        int cpu;
        pid_t pid = -1;
        char sbuf[STRERR_BUFSIZE];

        cpu = sched_getcpu();
        if (cpu < 0)
                cpu = 0;

        /*
         * Using -1 for the pid is a workaround to avoid gratuitous jump label
         * changes.
         */
        while (1) {
                /* check cloexec flag */
                fd = sys_perf_event_open(&attr, pid, cpu, -1,
                                         PERF_FLAG_FD_CLOEXEC);
                if (fd < 0 && pid == -1 && errno == EACCES) {
                        pid = 0;
                        continue;
                }
                break;
        }
        err = errno;

        if (fd >= 0) {
                close(fd);
                return 1;
        }

        WARN_ONCE(err != EINVAL && err != EBUSY,
                  "perf_event_open(..., PERF_FLAG_FD_CLOEXEC) failed with unexpected error %d (%s)\n",
                  err, strerror_r(err, sbuf, sizeof(sbuf)));

        /* not supported, confirm error related to PERF_FLAG_FD_CLOEXEC */
        while (1) {
                fd = sys_perf_event_open(&attr, pid, cpu, -1, 0);
                if (fd < 0 && pid == -1 && errno == EACCES) {
                        pid = 0;
                        continue;
                }
                break;
        }
        err = errno;

        if (fd >= 0)
                close(fd);

        if (WARN_ONCE(fd < 0 && err != EBUSY,
                      "perf_event_open(..., 0) failed unexpectedly with error %d (%s)\n",
                      err, strerror_r(err, sbuf, sizeof(sbuf))))
                return -1;

        return 0;
}

unsigned long perf_event_open_cloexec_flag(void)
{
        static bool probed;

        if (!probed) {
                if (perf_flag_probe() <= 0)
                        flag = 0;
                probed = true;
        }

        return flag;
}


#include <pthread.h>
pthread_mutex_t
pthread_create (thread, attr, start_routine, arg);

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);  //创建互斥量
int pthread_mutex_lock(pthread_mutex_t *mutex);//加锁
int pthread_mutex_unlock(pthread_mutex_t *mutex);//释放锁
int pthread_mutex_destroy(pthread_mutex_t *mutex);//销毁互斥量






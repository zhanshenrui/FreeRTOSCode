#include <pthread.h>
pthread_mutex_t
pthread_create (thread, attr, start_routine, arg);

int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);  //����������
int pthread_mutex_lock(pthread_mutex_t *mutex);//����
int pthread_mutex_unlock(pthread_mutex_t *mutex);//�ͷ���
int pthread_mutex_destroy(pthread_mutex_t *mutex);//���ٻ�����






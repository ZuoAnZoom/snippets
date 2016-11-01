/*
 * Thread Synchronization - Mutex
 * 
 * int pthread_cond_init(pthread_cond_t *cond, const pthread_cond_attr *attr)
 * int pthread_cond_destory(pthread_cond_t *cond)
 * int pthread_cond_wait(pthread_cond_t *cond, pthread_mutex_t *mutex)
 * int pthread_cond_timewait(pthread_cond_t *cond, pthread_mutex_t *mutex, const struct timespec *abstime)
 * int pthread_cond_signal(pthread_cond_t *cond)
 * int pthread_cond_broadcast(pthread_cond_t *cond)
 *
 * Copyright (c) 2016 Liming Shao <lmshao@163.com>
 */
 
#include <stdio.h>
#include <pthread.h>

#define SIZE 4
#define OVER (-1)

struct producer
{
	int buffer[SIZE];
	int rpos, wpos;
	pthread_mutex_t lock;
	pthread_cond_t notempty;
	pthread_cond_t notfull;
};

struct producer buffer;

void Init(struct producer *b)
{
	pthread_mutex_init(&b->lock, NULL);		//������
	pthread_cond_init(&b->notempty, NULL);	//��������
	pthread_cond_init(&b->notfull, NULL);	//��������
	b->rpos = 0;
	b->wpos = 0;
}

void Put(struct producer *b, int data)
{
	pthread_mutex_lock(&b->lock);	//lock
	
	while((b->wpos+1)%SIZE == b->rpos)	//����������ʱ����Ҫд��λ��
		pthread_cond_wait(&b->notfull, &b->lock);	//�ȴ�notfull�ź�
	
	b->buffer[b->wpos++] = data;

	if(b->wpos >= SIZE)
		b->wpos = 0;
	
	pthread_cond_signal(&b->notempty);	//����notempty�ź�
	pthread_mutex_unlock(&b->lock);
}

int Get(struct producer *b)
{
	pthread_mutex_lock(&b->lock);
	
	while(b->wpos == b->rpos)	//����������ʱ
		pthread_cond_wait(&b->notempty, &b->lock);	//�ȴ�notempty�ź�
	
	int data = b->buffer[b->rpos++];

	if(b->rpos >= SIZE) 
		b->rpos = 0;
	
	pthread_cond_signal(&b->notfull);	//����notfull�ź�
	pthread_mutex_unlock(&b->lock);
	
	return data;
}

void *Producer()
{
	int n;
	for(n=0; n<20; n++)
	{
		printf("Producer:%d-->\n",n);
		Put(&buffer, n);
	}
	Put(&buffer, OVER);
}

void *Consumer()
{
	for(;;)
	{
		int d = Get(&buffer);
		if(d == OVER)	
			break;
		printf("Consumer:-->%d\n", d);
	}
}

int main()
{
	pthread_t tha, thb;
	void *retval;
	Init(&buffer);
	pthread_create(&tha, NULL, Producer, 0);
	pthread_create(&thb, NULL, Consumer, 0);
	pthread_join(tha, &retval);
	pthread_join(thb, &retval);
	return 0;
}

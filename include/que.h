#ifndef QUE_H_INCLUDED
#define QUE_H_INCLUDED

typedef struct que_t {
    struct que_t *next;
    struct que_t *prev;
} que_t;

#define Q_EMPTY(q)            ((q)->next == (q))
#define Q_INSERT(q,data)      que_put((que_t *)q,data)
#define Q_NEW(q)              (q)->next = (q);(q)->prev = (q)
#define Q_NEXT(q)             ((void *)((que_t *)(q))->next)
#define Q_PREV(q)             ((void *)((que_t *)(q))->prev)

#define Q_WALK_MACRO(q,data)  for(data=Q_NEXT(q);(que_t *)data!=q;data=Q_NEXT(data))

#define Q_DEFINITION_MACRO(qname) que_t qname={&qname, &qname}

/*****************************************************************************/
static inline void q_remove (void *data)
{
    que_t *next = ((que_t *)data)->next;
    que_t *prev = ((que_t *)data)->prev;
    /* previous element's "next" points to next element */
    prev->next = next;
    /* next element's "prev" points to previous element */
    next->prev = prev;
}
/*****************************************************************************/
static inline void *q_get (void *q)
{
    que_t *qq   = (que_t *)q;
    que_t *data = qq->next;
    que_t *next = data->next;

    qq->next = next;
    next->prev = qq;

    return (data);
}
/*****************************************************************************/
static inline void q_put (void *q, void *data)
{
    que_t *dd   = (que_t *)data;
    que_t *qq   = (que_t *)q;
    que_t *prev = qq->prev;

    /* set new element pointers */
    dd->next = qq;
    dd->prev = prev;

    /* adjust othern element's pointers */
    prev->next = dd;
    qq->prev   = dd;

}


#define QUE_WALK_MACRO(q,data)      Q_WALK_MACRO(q,data)
#define QUE_DEFINITION_MACRO(qname) Q_DEFINITION_MACRO(qname)

/* TI CCS compatibility macros */

#define QUE_Elem               que_t
#define QUE_Obj                que_t
#define QUE_dequeue(q)         q_get(q)
#define QUE_get(q)             q_get(q)
#define QUE_put(q,data)        q_put(q,data)
#define QUE_enqueue(q,data)    q_put(q,data)
#define QUE_empty(q)           Q_EMPTY(q)
#define QUE_head(q)            Q_NEXT(q)
#define QUE_tail(q)            Q_PREV(q)
#define QUE_insert(q,data)     q_put(q,data)
#define QUE_new(q)             Q_NEW(q)
#define QUE_next(q)            Q_NEXT(q)
#define QUE_prev(q)            Q_PREV(q)
#define QUE_remove(data)       q_remove(data)

#endif /* QUE_*/

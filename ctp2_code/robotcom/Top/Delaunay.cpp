#include "c3.h"

#include "geom2d.h"
#include "Delaunay.h"

#include "Memory_Manager.h"


extern Memory_Manager *g_memory_QuadEdge;

void* QuadEdge::operator new(size_t byte_size)
{
    Assert(sizeof(QuadEdge)==byte_size);
    return g_memory_QuadEdge->Alloc();
}
void QuadEdge::operator delete(void *ptr)
{
    g_memory_QuadEdge->Dealloc(ptr);
}


inline QuadEdge::QuadEdge()
{
    m_qnext = NULL;
	e[0].num = 0, e[1].num = 1, e[2].num = 2, e[3].num = 3;
	e[0].next = &(e[0]); e[1].next = &(e[3]);
	e[2].next = &(e[2]); e[3].next = &(e[1]);
}


inline Edge* Edge::Rot()

{
	return (num < 3) ? this + 1 : this - 3;
}

inline Edge* Edge::invRot()

{
	return (num > 0) ? this - 1 : this + 3;
}

inline Edge* Edge::Sym()

{
	return (num < 2) ? this + 2 : this - 2;
}

inline Edge* Edge::Onext()

{
	return next;
}

inline Edge* Edge::Oprev()

{
	return Rot()->Onext()->Rot();
}

inline Edge* Edge::Dnext()

{
	return Sym()->Onext()->Sym();
}

inline Edge* Edge::Dprev()

{
	return invRot()->Onext()->invRot();
}

inline Edge* Edge::Lnext()

{
	return invRot()->Onext()->Rot();
}

inline Edge* Edge::Lprev()

{
	return Onext()->Sym();
}

inline Edge* Edge::Rnext()

{
	return Rot()->Onext()->invRot();
}

inline Edge* Edge::Rprev()

{
	return Sym()->Onext();
}


inline Point2d* Edge::Org()
{
	return data;
}

inline Point2d* Edge::Dest()
{
	return Sym()->data;
}

void Edge::EndPoints(Point2d* dont_name_things_or, Point2d* de)
{
	data = dont_name_things_or;
	Sym()->data = de;
}

inline const Point2d& Edge::Org2d() const
{
	return *data;
}

inline const Point2d& Edge::Dest2d() const
{
	return (num < 2) ? *((this + 2)->data) : *((this - 2)->data);
}

QuadEdge *g_quad_edge_head;
QuadEdge *g_quad_edge_tail;

Edge* MakeEdge()
{
	QuadEdge *ql = new QuadEdge;

    if (g_quad_edge_head == NULL) {
        g_quad_edge_head = ql;
        g_quad_edge_tail = ql;
    } else {
        g_quad_edge_tail->m_qnext = ql;
        g_quad_edge_tail = ql;
    }
	return ql->e;
}

void Splice(Edge* a, Edge* b)








{
	Edge* alpha = a->Onext()->Rot();
	Edge* beta  = b->Onext()->Rot();

	Edge* t1 = b->Onext();
	Edge* t2 = a->Onext();
	Edge* t3 = beta->Onext();
	Edge* t4 = alpha->Onext();

	a->next = t1;
	b->next = t2;
	alpha->next = t3;
	beta->next = t4;
}

void DeleteEdge(Edge* e)
{
	Splice(e, e->Oprev());
	Splice(e->Sym(), e->Sym()->Oprev());
	QuadEdge *del_me = e->Qedge();

    QuadEdge *q;
    if (del_me == g_quad_edge_head) {
        if (del_me == g_quad_edge_tail) {
            g_quad_edge_tail = NULL;
        }
        g_quad_edge_head = g_quad_edge_head->m_qnext;
    } else {
        for (q = g_quad_edge_head; q && (q->m_qnext != del_me); q = q->m_qnext);

        Assert(q);
        Assert(q->m_qnext == del_me);

        if (g_quad_edge_tail == del_me) {
            g_quad_edge_tail = q;
        }
        q->m_qnext = del_me->m_qnext;
    }
    delete del_me;
}






Subdivision::Subdivision(const Point2d& a, const Point2d& b, const Point2d& c)

{
	Point2d *da, *db, *dc;
	da = new Point2d(a), db = new Point2d(b), dc = new Point2d(c);
	Edge* ea = MakeEdge();
	ea->EndPoints(da, db);
	Edge* eb = MakeEdge();
	Splice(ea->Sym(), eb);
	eb->EndPoints(db, dc);
	Edge* ec = MakeEdge();
	Splice(eb->Sym(), ec);
	ec->EndPoints(dc, da);
	Splice(ec->Sym(), ea);
	startingEdge = ea;
}

Edge* Connect(Edge* a, Edge* b)




{
	Edge* e = MakeEdge();
	Splice(e, a->Lnext());
	Splice(e->Sym(), b);
	e->EndPoints(a->Dest(), b->Org());
	return e;
}

void Swap(Edge* e)

{
	Edge* a = e->Oprev();
	Edge* b = e->Sym()->Oprev();
	Splice(e, a);
	Splice(e->Sym(), b);
	Splice(e, a->Lnext());
	Splice(e->Sym(), b->Lnext());
	e->EndPoints(a->Dest(), b->Dest());
}


inline Real TriArea(const Point2d& a, const Point2d& b, const Point2d& c)

{
	return (b.x - a.x)*(c.y - a.y) - (b.y - a.y)*(c.x - a.x);
}

int InCircle(const Point2d& a, const Point2d& b,
			 const Point2d& c, const Point2d& d)

{
	return (a.x*a.x + a.y*a.y) * TriArea(b, c, d) -
	       (b.x*b.x + b.y*b.y) * TriArea(a, c, d) +
	       (c.x*c.x + c.y*c.y) * TriArea(a, b, d) -
	       (d.x*d.x + d.y*d.y) * TriArea(a, b, c) > 0;
}

int ccw(const Point2d& a, const Point2d& b, const Point2d& c)

{
	return (TriArea(a, b, c) > 0);
}

int RightOf(const Point2d& x, Edge* e)
{
	return ccw(x, e->Dest2d(), e->Org2d());
}

int LeftOf(const Point2d& x, Edge* e)
{
	return ccw(x, e->Org2d(), e->Dest2d());
}

int OnEdge(const Point2d& x, Edge* e)


{
	Real t1, t2, t3;
	t1 = (x - e->Org2d()).norm();
	t2 = (x - e->Dest2d()).norm();
	if (t1 < EPS || t2 < EPS)
	    return TRUE;
	t3 = (e->Org2d() - e->Dest2d()).norm();
	if (t1 > t3 || t2 > t3)
	    return FALSE;
	Line line(e->Org2d(), e->Dest2d());
	return (fabs(line.eval(x)) < EPS);
}




Edge* Subdivision::Locate(const Point2d& x)




{
	Edge* e = startingEdge;

#ifdef _DEBUG
    sint32 finite_loop=0;
#endif

	while (TRUE) {
Assert(++finite_loop < 100000);
		if (x == e->Org2d() || x == e->Dest2d())
		    return e;
		else if (RightOf(x, e))
			 e = e->Sym();
		else if (!RightOf(x, e->Onext()))
			 e = e->Onext();
		else if (!RightOf(x, e->Dprev()))
			 e = e->Dprev();
		else
		    return e;
	}
}

void Subdivision::InsertSite(const Point2d& x)





{
	Edge* e = Locate(x);
	if ((x == e->Org2d()) || (x == e->Dest2d()))
	    return;
	else if (OnEdge(x, e)) {
		e = e->Oprev();
		DeleteEdge(e->Onext());
	}




	Edge* base = MakeEdge();
	base->EndPoints(e->Org(), new Point2d(x));
	Splice(base, e);
	startingEdge = base;

#ifdef _DEBUG
    sint32 finite_loop=0;
#endif

	do {
		base = Connect(e, base->Sym());
		e = base->Oprev();
Assert(++finite_loop < 100000);
	} while (e->Lnext() != startingEdge);


	do {
		Edge* t = e->Oprev();
		if (RightOf(t->Dest2d(), e) &&
			InCircle(e->Org2d(), t->Dest2d(), e->Dest2d(), x)) {
				Swap(e);
				e = e->Oprev();
		}
		else if (e->Onext() == startingEdge)
			return;
		else
		    e = e->Onext()->Lprev();

Assert(++finite_loop < 100000);

	} while (TRUE);
}

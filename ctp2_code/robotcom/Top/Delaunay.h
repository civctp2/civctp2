
#pragma once

#ifndef QUADEDGE_H
#define QUADEDGE_H 1

class QuadEdge;
class FS_MinSpanTree; 

class Edge {
	friend QuadEdge;
    friend FS_MinSpanTree; 
	friend void Splice(Edge*, Edge*);
  private:
	int num;
   	Edge *next;
	Point2d *data;
  public:
	Edge()			{ data = 0; }
    ~Edge() {} 

	Edge* Rot();
	Edge* invRot();
	Edge* Sym();
	Edge* Onext();
	Edge* Oprev();
	Edge* Dnext();
	Edge* Dprev();
	Edge* Lnext();
	Edge* Lprev();
	Edge* Rnext();
	Edge* Rprev();
	Point2d* Org();
	Point2d* Dest();
	const Point2d& Org2d() const;
	const Point2d& Dest2d() const;
	void  EndPoints(Point2d*, Point2d*);
	QuadEdge* Qedge()	{ return (QuadEdge *)(this - num); }
	void Draw(unsigned int);

};

class QuadEdge; 
class FS_MinSpanTree;

class QuadEdge {
    friend FS_MinSpanTree;
	friend Edge *MakeEdge();
    friend void DeleteEdge(Edge* e);

  private:
	Edge e[4];
    QuadEdge *m_qnext; 
  public:
	QuadEdge();
    ~QuadEdge() {} 

    
    void* operator new(size_t); 
    void operator delete(void *);
};

class Subdivision {
  private:
	Edge *startingEdge;
	Edge *Locate(const Point2d&);
  public:
	Subdivision(const Point2d&, const Point2d&, const Point2d&);
	void InsertSite(const Point2d&);
	void Draw();
};

#endif 

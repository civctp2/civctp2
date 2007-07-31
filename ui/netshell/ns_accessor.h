

















#ifndef __NS_ACCESSOR_H__
#define __NS_ACCESSOR_H__




























template<class T>
class ns_Accessor {
public:
	typedef void* (T::*Data);
	enum Type {
		INT,
		STRING,
		ICON,
		ERR
	};
	struct Struct {
		Type type;
		Data data;
	};
	static int count;
	static Struct list[];
	Type type(int i) {
		if(i < count)
			return list[i].type;
		else
			return ERR;
	}
	void *data(int i) {
		if (i < count)
			return ((T *)this)->*(list[i].data);
		else
			return NULL;
	}
	int comp(int i, T *p) {
		Type t = ((T *)this)->type(i);
		void *a = ((T *)this)->data(i);
		void *b = p->data(i);
		switch(t) {
		case INT:
			return (int)a - (int)b;
		case STRING:
		case ICON:
			if(a) {
				if(b)
					return strcmp((char *)a, (char *)b);
				else
					return 1;
			} else {
				if(b)
					return -1;
				else
					return 0;
			}
		}
		return 0;
	}
};


#endif 

/*******************************************************************************

	$Workfile:: ldl_attr.hpp                                                   $
	  $Author$
	    $Date$
	$Rev$
	 $Archive:: /LDL/test/ldl_attr.hpp                                         $

*******************************************************************************/


#ifndef LDL_ATTR_HPP
#define LDL_ATTR_HPP

enum ATTRIBUTE_TYPE {
	ATTRIBUTE_TYPE_UNKNOWN,
	ATTRIBUTE_TYPE_INT,
	ATTRIBUTE_TYPE_DOUBLE,
	ATTRIBUTE_TYPE_STRING,
	ATTRIBUTE_TYPE_BOOL,
};

#define k_MAX_ATTRIBUTE_LEN		32

class ldl;

class ldl_attribute {
  protected:
	char *m_name;
	ATTRIBUTE_TYPE m_type;
	ldl_attribute *m_next;
	friend class ldl_attributelist;

  public:
	ldl_attribute(char *name, ATTRIBUTE_TYPE type) { m_name = name; m_type = type; m_next = NULL; }

	ldl_attribute *GetCopy();

	ATTRIBUTE_TYPE GetType() { return m_type; }
	char *GetName() { return m_name; }
	const char *GetTypeName() {
		switch(m_type) {
			case ATTRIBUTE_TYPE_BOOL: return "bool";
			case ATTRIBUTE_TYPE_INT:  return "int";
			case ATTRIBUTE_TYPE_DOUBLE: return "double";
			case ATTRIBUTE_TYPE_STRING: return "string";
			default: return "BADTYPE";
		}
	}

	bool GetBoolValue();
	int GetIntValue();
	double GetFloatValue();
	char *GetStringValue();

	char *GetValueText();
};

template <class Type> class ldl_attributeValue : public ldl_attribute {
  private:
	Type m_value;

  public:
	ldl_attributeValue(char *name, ATTRIBUTE_TYPE t, Type value) : ldl_attribute(name, t) 
	{
		m_value = value;
	}

	ldl_attributeValue(ldl_attribute *copy) : ldl_attribute(copy->GetName(), copy->GetType()) {
		m_value = ((ldl_attributeValue<Type> *)copy)->m_value;
	}

	Type GetValue() { return m_value; }
	void SetValue(Type val) { m_value = val; }
};


#endif //LDL_ATTR_HPP

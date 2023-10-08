#ifndef DUMMY_NODE_H
#define DUMMY_NODE_H

#include "node.h"

class DummyNode : public Node {
	
	IAPI_TYPE( DummyNode, Node );
	IAPI_INSTANCE_FUNC( DummyNode );
	
public:
	
	DummyNode();
	~DummyNode();
};




#endif

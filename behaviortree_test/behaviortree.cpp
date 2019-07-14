/* behaviortree.cpp
 * by Germain Sauvé
 * 
 * IN DEVELOPMENT - NOT COMPLETE
 * 
 */
 
#include "behaviortree.h"

void BlackBoard::init()
{
  for ( int i = 0; i < __MAXBBELEMENTS__ - 1; ++i )
  {
    elements[i] = 0;
    elemKey[i] = BLACKBOARD_NOKEY;
    elemState[i] = 0;
  }
}

int BlackBoard::setNewElement( int value )
{
  // look for an available index 

  // if OK, random key,  while haskey, key = random

  // return key
  return BLACKBOARD_NOKEY;
}

boolean BlackBoard::releaseElement( int key)
{
  // find key index, set state to none, value to 0 and key to BLACKBOARD_NOKEY
  return false;
}

boolean BlackBoard::hasKey(int key )
{
  // search for key, return true if found in use
  return false;
}

int BlackBoard::get( int key )
{
  // find key index, return value
  return 0;
}

boolean BlackBoard::set(int key, int value )
{
  // find free ements index, set key and value
  return false;
}

boolean BlackBoard::forceElementValue( int key, int value )
{
  // find least used or other mnemonic elem to release and set from index
  return false;
}

void BehaviorBank::init( const byte* sizes, const int* indexes, const char* datas, int total )
{
  this->totalElements = total;
  this->behaviorIndexesSizes = sizes;
  this->behaviorIndexes = indexes;
  this->behaviorDatas = datas;
}

byte BehaviorBank::getNbrNodes( int idx )
{
  byte ret = 0;
  if ( idx <= this->totalElements )
  {
    ret = pgm_read_byte_near( this->behaviorIndexesSizes + idx );
    //Serial.print("nbrNodes:");
    //Serial.println(ret);
  }
  return ret;
}

char* BehaviorBank::getDataPtr( int idx )
{
   if ( idx <= this->totalElements )
  {
    int pos = pgm_read_word_near(this->behaviorIndexes + idx);
    //Serial.print("idx:");
    //Serial.print(idx);
    //Serial.print(":pos:");
    //Serial.println(pos);
    return this->behaviorDatas + pos;
  }
  return NULL;
}


void BehaviorTreeNode::init( byte type, int data)
{
  this->type = type;
  this->data = data;
  this->next = BEHAVE_NODE_NO_INDEX;
  this->child = BEHAVE_NODE_NO_INDEX;
  this->setState(NODE_STATUS_UNTOUCH);
  this->setPriority(0);
}
 
void BehaviorTreeNode::clear()
{
  this->type = BEHAVE_NOTYPE;
  this->data = 0;
  this->next = BEHAVE_NODE_NO_INDEX;
  this->child = BEHAVE_NODE_NO_INDEX;
  this->setState(NODE_STATUS_UNTOUCH);
  this->setPriority(0);
}

#ifdef __DEBUG__
void BehaviorTreeNode::debugPrint(byte depth)
{
  for (int i=0; i<depth; ++i )
  {
    Serial.print('-');
  }
  Serial.print(' ');
  Serial.print(this->type);
  Serial.print(':');
  Serial.print(this->data);
  Serial.print(':');
  Serial.print(this->getState());
  Serial.print(':');
  Serial.print(this->getPriority());
  Serial.print(':');
  Serial.print(this->child);
  Serial.print(':');
  Serial.println(this->next);
}
#endif 



BehaviorTreeVisitor::BehaviorTreeVisitor()
{
  this->nodes = NULL;
  this->maxNode = 0;
  this->currentNode = 0;
  this->endnode = BEHAVE_NODE_NO_INDEX;
  this->depthvisited[0] = 0;
  this->depth = 0;
}

BehaviorTreeVisitor::BehaviorTreeVisitor(BehaviorTreeNode* n, byte maxn, byte start)
{
  this->init(n, maxn, start);
}

void BehaviorTreeVisitor::init(BehaviorTreeNode* n, byte maxn, byte start)
{
  this->nodes = n;
  this->maxNode = maxn;
  this->currentNode = start;
  this->endnode = BEHAVE_NODE_NO_INDEX;
  this->depthvisited[0] = start;
  this->depth = 0;
}

boolean BehaviorTreeVisitor::hasReachEnd()
{
  if ( this->depth == 0 && this->current()->next == BEHAVE_NODE_NO_INDEX)
  {
    if ( this->current()->child != BEHAVE_NODE_NO_INDEX && this->currentNode != endnode )
    {
      // pas encore visiter l'enfant
      return false;
    }
    return true;
  }
  return false;
}
  
boolean BehaviorTreeVisitor::moveNext()
{
  if ( this->hasNext() )
  {
    this->currentNode = this->current()->next;
    // Serial.print("Move next to ");
    // Serial.println(this->currentNode);
    return true;
  }
  // Serial.println("Can't move next");
  return false;
}
  
boolean BehaviorTreeVisitor::moveUp()
{
  if ( depth > 0)
  {
    this->currentNode = this->depthvisited[--depth];
    // Serial.print("Move up to ");
    // Serial.println(this->currentNode);
    return true;
  }
  // Serial.println("Can't move Up");
  return false;
}
  
boolean BehaviorTreeVisitor::moveDown()
{
  if ( this->hasChild() )
  {
    this->currentNode = this->current()->child;
    if ( depth == 0)
    {
      endnode = this->currentNode;
    }
    this->depthvisited[++depth] = this->currentNode;
    // Serial.print("Move down to ");
    // Serial.println(this->currentNode);
    return true;
  }
  // Serial.println("Can't move down");
  return false;
}


boolean BehaviorTreeVisitor::moveToChild( byte idx )
{
  // non optimzed
  // move down then next idx times
  if ( this->getChildLength() > idx )
  {
    this->moveDown();
    while( idx-- > 0)
    {
      this->moveNext();
    }
    return true;
  }
  return false;
};

// simplifie l'acces au node
boolean BehaviorTreeVisitor::hasNext()
{
  return this->current()->next != BEHAVE_NODE_NO_INDEX;
}
  
boolean BehaviorTreeVisitor::hasChild()
{
  return this->current()->child != BEHAVE_NODE_NO_INDEX;
}
  
byte BehaviorTreeVisitor::getChildLength()
{
  byte ret = 0;
  byte currentidx = 0;
  // if has child, add 1
  if ( this->hasChild() )
  {
      ret += 1;
      currentidx = this->current()->child;
      // while has next, add 1, move next
      while ( this->nodes[currentidx].next != BEHAVE_NODE_NO_INDEX )
      {
        ret += 1;
        currentidx = this->nodes[currentidx].next;
      }
  }
  //Serial.print("NbrChild:");
  //Serial.println(ret);
  return ret;
}


int BehaviorTree::popFree()
{
  if ( this->nbrFree <= 0 )
  {
    // pas de place!
    return -1;
  }
  
  // retourne le dernier element a free
  this->nbrFree -= 1;
  int ret = this->freeNodes[this->nbrFree];
  return ret;
}

void BehaviorTree::PushFree(int idx)
{
  // set le flag a nodes
  this->freeNodes[this->nbrFree] = idx;
  this->nbrFree += 1;
}

void BehaviorTree::init()
{
  this->nbrFree = __MAXBEHAVIORTREENODE__;
  this->root = BEHAVE_NODE_NO_INDEX;
  for ( int i =0; i< __MAXBEHAVIORTREENODE__; ++i)
  {
    this->nodes[i].state = BEHAVE_NODE_NO_INDEX;
    this->freeNodes[i] = __MAXBEHAVIORTREENODE__ - 1 - i;
  }
}

int BehaviorTree::setRoot(byte type, int data)
{
  // Si root fonctionne????
  int ret = this->popFree();
  if ( ret < 0)
    return ret;
  
  this->root = ret;
  this->nodes[ret].init( type, data);
  return ret;
}

// Insertion
int BehaviorTree::addChild( byte parent, byte type, int data )
{
  int ret = this->popFree();
  if ( ret < 0)
    return ret;
  
  // si pas d'enfant, prend la place, sinon va a la fin des enfants
  //Serial.print("addchild ");
  //Serial.print(ret);
  //Serial.print(" to ");
  //Serial.println(parent);
  this->nodes[ret].init(type, data);
  this->nodes[ret].next = this->nodes[parent].child;
  this->nodes[parent].child = ret;
  return ret;
}

int BehaviorTree::addNext( byte previous, byte type, int data )
{
  int ret = this->popFree();
  if ( ret < 0)
    return ret;
  //Serial.print("addnext ");
  //Serial.print(ret);
  //Serial.print(" to ");
  //Serial.println(previous);
  // garde next en memoire, prend la place et set le nouveau avec ancien next
  this->nodes[ret].init( type, data);
  this->nodes[ret].next = this->nodes[previous].next;
  this->nodes[previous].next = ret;
  return ret;
}


void BehaviorTree::deleteNode( byte parent, byte node)
{
  if ( this->nodes[node].child != BEHAVE_NODE_NO_INDEX)
  {
    this->deleteNode(node, this->nodes[node].child);
  }
  if ( this->nodes[node].next != BEHAVE_NODE_NO_INDEX)
  {
    this->deleteNode(parent, this->nodes[node].next);
  }
  this->nodes[parent].child = BEHAVE_NODE_NO_INDEX;
  this->nodes[node].clear();
  this->PushFree(node);
}

boolean BehaviorTree::deleteChildNode( byte parent, byte node)
{
  //Serial.print("p:");
  //Serial.print(parent);
  //Serial.print(":c:");
  //Serial.println(node);
  //
  if ( this->nodes[node].child != BEHAVE_NODE_NO_INDEX)
  {
    //Serial.println("delchild");
    this->deleteNode(node, this->nodes[node].child);
  }
  if (this->nodes[node].next != BEHAVE_NODE_NO_INDEX)
  {
    //Serial.println("delnext");
    // start from parent, find previous then relink
    if ( this->nodes[parent].child == node )
    {
      this->nodes[parent].child = this->nodes[node].next;
    }
    else
    {
      byte curprev = this->nodes[parent].child;
      while ( this->nodes[curprev].next != node && curprev != BEHAVE_NODE_NO_INDEX )
      {
        curprev = this->nodes[curprev].next;
      }
      if ( curprev != BEHAVE_NODE_NO_INDEX )
      {
        this->nodes[curprev].next = this->nodes[node].next;
      }
    }
  }
  else
  {
    //Serial.println("delnonext");
    this->nodes[parent].child = BEHAVE_NODE_NO_INDEX;
  }
  this->nodes[node].clear();
  this->PushFree(node);
  return true;
}



boolean BehaviorTree::deserialize( byte nodeparent, const byte* data )
{
 byte nstack[__MAXBTREEDEPTHVISITOR__]; // Max depth de 16
 byte depth = 0;
  
  char* curPtr = (char*)data;
  
  byte btype = ((byte*)curPtr)[0];
  int bdata = ((int*)(curPtr + 1))[1];
  int ret = this->addChild(nodeparent, btype, bdata);
  int curnode = ret;
  
  boolean end = false;
  int nextact = curPtr[3];
  curPtr = curPtr + 4;
  while( !end )
  {
    //Serial.println(nextact);
    int btype = ((byte*)curPtr)[0];
    int bdata = ((int*)(curPtr + 1))[0];
    // si child, push, add child
    if (nextact == 0)
    {
      end = true;
    }
    else if (nextact == 1)
    {
      // add next
      curnode = this->addNext(curnode, btype, bdata);
    }
    else if (nextact == 2)
    {
      // push
      nstack[depth] = curnode;
      depth += 1;
      // add child
      curnode = this->addChild(curnode, btype, bdata);
    }
    else if (nextact < 0)
    {
      // pop de next hack
      depth += nextact;
      curnode = nstack[depth];
      // add next
      curnode = this->addNext(curnode, btype, bdata);
    }
    nextact = curPtr[3];
    curPtr = curPtr + 4;
  }
  return ret;
}

boolean BehaviorTree::deserialize_flash(byte nodeparent, const byte* data )
{
  byte nstack[__MAXBTREEDEPTHVISITOR__]; // Max depth of 16
 byte depth = 0;
  byte nextv;
  char* curPtr = (char*)data;
  
  byte btype = pgm_read_byte_near(curPtr);
  int bdata = pgm_read_word_near(curPtr + 1);
  int ret = this->addChild(nodeparent, btype, bdata);
  int curnode = ret;
  
  boolean end = false;
  int nextact;
  // ugly cast to have signed byte from pgm_read_byte_near cast into an int
  nextv = pgm_read_byte_near(curPtr +3);
  nextact = ((char *)(&nextv))[0];
  curPtr = curPtr + 4;
  while( !end )
  {
    //Serial.println(nextact);
    int btype = pgm_read_byte_near(curPtr);
    int bdata = pgm_read_word_near(curPtr + 1);
    // si child, push, add child
    if (nextact == 0)
    {
      end = true;
    }
    else if (nextact == 1)
    {
      // add next
      curnode = this->addNext(curnode, btype, bdata);
    }
    else if (nextact == 2)
    {
      // push
      nstack[depth] = curnode;
      depth += 1;
      // add child
      curnode = this->addChild(curnode, btype, bdata);
    }
    else if (nextact < 0)
    {
      // pop de next hack
      depth += nextact;
      curnode = nstack[depth];
      // add next
      curnode = this->addNext(curnode, btype, bdata);
    }
    nextv = pgm_read_byte_near(curPtr +3);
    nextact = ((char *)(&nextv))[0];
    curPtr = curPtr + 4;
  }
  return ret;
}

boolean BehaviorTree::deserialize( const byte* data )
{
   byte nstack[__MAXBTREEDEPTHVISITOR__]; // Max depth de 16
 byte depth = 0;
  
  char* curPtr = (char*)data;
  
  byte btype = ((byte*)curPtr)[0];
  int bdata = ((int*)(curPtr + 1))[1];
  int ret = this->setRoot(btype, bdata);
  int curnode = ret;
  
  boolean end = false;
  int nextact = curPtr[3];
  curPtr = curPtr + 4;
  while( !end )
  {
    //Serial.println(nextact);
    int btype = ((byte*)curPtr)[0];
    int bdata = ((int*)(curPtr + 1))[0];
    // si child, push, add child
    if (nextact == 0)
    {
      end = true;
    }
    else if (nextact == 1)
    {
      // add next
      curnode = this->addNext(curnode, btype, bdata);
    }
    else if (nextact == 2)
    {
      // push
      nstack[depth] = curnode;
      depth += 1;
      // add child
      curnode = this->addChild(curnode, btype, bdata);
    }
    else if (nextact < 0)
    {
      // pop de next hack
      depth += nextact;
      curnode = nstack[depth];
      // add next
      curnode = this->addNext(curnode, btype, bdata);
    }
    nextact = curPtr[3];
    curPtr = curPtr + 4;
  }
  return ret;
}

boolean BehaviorTree::deserialize_flash( const byte* data )
{
  byte nstack[__MAXBTREEDEPTHVISITOR__]; // Max depth de 16
 byte depth = 0;
  byte nextv;
  char* curPtr = (char*)data;
  
  byte btype = pgm_read_byte_near(curPtr);
  int bdata = pgm_read_word_near(curPtr + 1);
  int ret = this->setRoot(btype, bdata);
  int curnode = ret;
  
  boolean end = false;
  int nextact;
  // ugly cast to have signed byte from pgm_read_byte_near cast into an int
  nextv = pgm_read_byte_near(curPtr +3);
  nextact = ((char *)(&nextv))[0];
  curPtr = curPtr + 4;
  while( !end )
  {
    // Serial.println(nextact);
    int btype = pgm_read_byte_near(curPtr);
    int bdata = pgm_read_word_near(curPtr + 1);
    // si child, push, add child
    if (nextact == 0)
    {
      end = true;
    }
    else if (nextact == 1)
    {
      // add next
      curnode = this->addNext(curnode, btype, bdata);
    }
    else if (nextact == 2)
    {
      // push
      nstack[depth] = curnode;
      depth += 1;
      // add child
      curnode = this->addChild(curnode, btype, bdata);
    }
    else if (nextact < 0)
    {
      // pop de next hack
      depth += nextact;
      curnode = nstack[depth];
      // add next
      curnode = this->addNext(curnode, btype, bdata);
    }
    nextv = pgm_read_byte_near(curPtr +3);
    nextact = ((char *)(&nextv))[0];
    curPtr = curPtr + 4;
  }
  return ret;
}


boolean BehaviorTree::fillSubNodeState(byte node, byte state)
{
  this->nodes[node].setState(state);
  if (this->nodes[node].child != BEHAVE_NODE_NO_INDEX )
  {
    this->fillSubNodeState(this->nodes[node].child, state);
  }
  if (this->nodes[node].next != BEHAVE_NODE_NO_INDEX )
  {
    this->fillSubNodeState(this->nodes[node].next, state);
  }  
  return false; 
}

boolean BehaviorTree::fillSubTreeState(byte parent, byte state)
{
  if ( this->nodes[parent].child != BEHAVE_NODE_NO_INDEX )
  {
    this->fillSubNodeState(this->nodes[parent].child, state);
  }
  return false; 
}

boolean BehaviorTree::fillSubNodePriority(byte node, byte priority)
{
  this->nodes[node].setPriority(priority);
  if (this->nodes[node].child != BEHAVE_NODE_NO_INDEX )
  {
    this->fillSubNodePriority(this->nodes[node].child, priority);
  }
  if (this->nodes[node].next != BEHAVE_NODE_NO_INDEX )
  {
    this->fillSubNodePriority(this->nodes[node].next, priority);
  }  
  return false; 
}

boolean BehaviorTree::fillSubTreePriority(byte parent, byte priority)
{
 if ( this->nodes[parent].child != BEHAVE_NODE_NO_INDEX )
  {
    this->fillSubNodePriority(this->nodes[parent].child, priority);
  }
  return false;  
}
  
// Visitor
void BehaviorTree::initVisitor( BehaviorTreeVisitor& visitor )
{
  visitor.init( this->nodes, __MAXBEHAVIORTREENODE__, this->root);
}



// management
void BehaviorTree::clean()
{
  // go trhough all tree nodes ans delete where state is delete
  for ( int i = 0; i < __MAXBEHAVIORTREENODE__; ++i )
  {
      if ( this->nodes[i].state == NODE_STATUS_DELETE )
      {
        // pushFree
        this->PushFree(i);
        // change state to UNTOUCH
        this->nodes[i].state = NODE_STATUS_UNTOUCH;
      }
  }
  return;  
}


#ifdef __DEBUG__
void BehaviorTree::debugPrintNode(byte node, byte depth)
{
  Serial.print(node);
  this->nodes[node].debugPrint(depth);
  if (this->nodes[node].child != BEHAVE_NODE_NO_INDEX )
  {
    this->debugPrintNode(this->nodes[node].child, depth + 1);
  }
  if (this->nodes[node].next != BEHAVE_NODE_NO_INDEX )
  {
    this->debugPrintNode(this->nodes[node].next, depth);
  }  
}

void BehaviorTree::debugPrint()
{
  Serial.println("BehaviorTree");
  Serial.print("Nbr Free:");
  Serial.println(this->nbrFree);
  Serial.print("Root:");
  Serial.println(this->root);
  this->debugPrintNode(this->root, 0);
}

#endif 

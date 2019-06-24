/* behaviortree.cpp
 * by Germain Sauvé
 * 
 * IN DEVELOPMENT - NOT COMPLETE
 * 
 */
 
#include "behaviortree.h"

void BehaviorTreeNode::init( byte type, int data)
{
  this->type = type;
  this->data = data;
  this->next = BEHAVE_NODE_NO_INDEX;
  this->child = BEHAVE_NODE_NO_INDEX;
  this->state = NODE_STATUS_UNTOUCH;
}
 
void BehaviorTreeNode::clear()
{
  this->type = BEHAVE_NOTYPE;
  this->data = 0;
  this->next = BEHAVE_NODE_NO_INDEX;
  this->child = BEHAVE_NODE_NO_INDEX;
  this->state = NODE_STATUS_UNTOUCH;
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
  Serial.println(this->state);
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
    return true;
  }
  return false;
}
  
boolean BehaviorTreeVisitor::moveUp()
{
  if ( depth > 0)
  {
    this->currentNode = this->depthvisited[--depth];
    return true;
  }
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
    return true;
  }
  return false;
}
  

// simplifie l'acces au node
boolean BehaviorTreeVisitor::hasNext()
{
  return this->current()->next != BEHAVE_NODE_NO_INDEX;
}
  
boolean BehaviorTreeVisitor::hasChild()
{
  return this->current()->child != BEHAVE_NODE_NO_INDEX;
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
boolean BehaviorTree::addChild( byte parent, byte type, int data )
{
  int ret = this->popFree();
  if ( ret < 0)
    return ret;
  
  // si pas d'enfant, prend la place, sinon va a la fin des enfants
  
  this->nodes[ret].init( type, data);
  this->nodes[ret].next = this->nodes[parent].child;
  this->nodes[parent].child = ret;
  return ret;
}

boolean BehaviorTree::addNext( byte previous, byte type, int data )
{
  int ret = this->popFree();
  if ( ret < 0)
    return ret;
  
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
  //
  if ( this->nodes[node].child != BEHAVE_NODE_NO_INDEX)
  {
    this->deleteNode(node, this->nodes[node].child);
  }
  if (this->nodes[node].next != BEHAVE_NODE_NO_INDEX)
  {
    this->nodes[parent].child = this->nodes[node].next;
  }
  else
  {
    this->nodes[parent].child = BEHAVE_NODE_NO_INDEX;
  }
  this->nodes[node].clear();
  this->PushFree(node);
  return true;
}



boolean BehaviorTree::deserialize( byte nodeparent, byte* data )
{
 byte nstack[__MAXBTREEDEPTHVISITOR__]; // Max depth de 16
 byte depth = 0;
  
  char* curPtr = (char*)data;
  
  byte btype = ((byte*)data)[0];
  int bdata = ((int*)data)[1];
  int ret = this->setRoot(btype, bdata);
  int curnode = ret;
  
  boolean end = false;
  int nextact = curPtr[3];
  curPtr = curPtr + 4;
  while( !end )
  {
    Serial.println(nextact);
    int btype = ((byte*)curPtr)[0];
    int bdata = ((int*)curPtr)[1];
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

boolean BehaviorTree::deserialize_flash(byte nodeparent, byte* data )
{
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
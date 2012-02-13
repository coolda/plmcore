/* stack.h */

/* definition of a class template for a stack */
/* The declarations and definitions of */
/* the class template member functions */
/* should all be in the same header file */

template <class T>	class Stack
{
  public:
	Stack( float = 64 );
	~Stack()
	{
		delete []stackPtr;
	}
	int push(const T&);
	int pop(T&); 	// you need param for a pop? prepare a var for receive it
	int isEmpty()const
	{
		return (top == -1);		// return 1 as a true in this case
	}
	int isFull()const
	{
		return (top == size - 1 );
	}
	
  private:
	int size; // number of elements on Stack
	int top;
	T* stackPtr;
};

// constructor with the default size 10
template <class T> Stack<T>::Stack(float s)
{
	size = s > 0 && s < 2048 ?s:64;
	top = -1;	// initialize stack
	stackPtr = new T[size];
}

// push an element onto the stack
template <class T> int Stack<T>::push(const T& item)
{
	if (! isFull() )
	{
		stackPtr[++top] = item;
		return 1 ;		// push successful
	}
	return 0 ;		// push failed it is full
}

// pop an element off the stack
template <class T> int Stack<T>::pop(T& popValue)
{
	if (!isEmpty() )
	{
		popValue = stackPtr[top--] ;
		return 1 ;	// pop success
	}
	return 0 ;		// pop failed it is empty
}

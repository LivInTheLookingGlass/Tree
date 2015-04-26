# Tree (Experimental)
An alternative, versatile database based off the Red Black Tree

---

## Dependencies

Boost 1.5.3 or greater

C++11 or greater

## API

**Constructor**

    ALTTree T = ALTTree();
The default constructor will create an instance of the tree, and a threadpool to manage it. The use of the threadpool is optional, and an additional constructor will be added which does not include the threadpool.

    ALTTree T = ALTTree(ALTNode *r);
This constructor will transform a subtree into a full tree. This would allow for its use in distributed data structures, or for cloning.

**Methods**

    T.add(string key, string data);
    T += (string data); (not yet implemented)
This method allows you to add an item to the tree. It will store the key and data, sorted in order of key. If this is going to be used as a linked list or array, I strongly recommend using an identical key and data field. This will not let you use the built in modification methods (yet), but it will be much more convenient for array accessing.

    T.remove(string key);
    T -= (string key);
    T.remove(unsigned long long index); (not yet implemented)
    T -= (unsigned long long index);    (not yet implemented)
This method allows you to remove any item from the tree, either by ordered index, or by key. To remove by index for now, you need to use the workaround of T -= T[index];

    T.search(string key);
    T.search(unsigned long long index);
    T[unsigned long long index];
    T(unsigned long long chronological_index);  (not yet implemented)
This method allows you to get the data from any known key or index.

    T[string key]
    T(string key)   (not yet implemented)
These methods allow you to get the index of an item by its key. The first will get you the sorted index, and the second will retrieve its chronological index.

    T.Queue(int cmd, string data);
This method will queue up any command into the built-in threadpool. Currently supported commands are 0 for remove, and 1 for add. Addition data format is "key,data". This will be changed to a pair<string,string> in a future release to allow for easier compatibility.

    T.print();
    T.rprint();
    T.cprint();
    T.crprint();
    T.tprint();   (not yet implemented)
    T.trprint();  (not yet implemented)
Each of these will print all nodes in a different format. The first two will print in sorted order. The second pair will print in chronological order. The last pair will print in a tree-formatted manner, similar to in DOS.

    T.pauseQueue();
If you want your program to work idly, this is the easiest way to do it. Calling this method will put the threadpool into sleep mode, only checking their "alive" flag every 10 milliseconds.

    T.startQueue();
This method will undo the action from the above. It will have the threadpool constantly poll the command queue, at a rate of once per 0.25 milliseconds

There are more coming, but their support is currently sparse enough as to not deserve over-mention. They will include:

* FIFO access support
* LIFO access support
* Linked List accesss support
  * Circular Linked List accesss support
* Quick undo command (addition only, so far)
* Operator overload for push
* Operator overload for pull (possibly)

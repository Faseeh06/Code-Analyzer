class Node<T> {
    data: T;
    next: Node<T> | null;
  
    constructor(data: T) {
      this.data = data;
      this.next = null;
    }
  }
  
  export class LinkedList<T> {
    head: Node<T> | null;
    tail: Node<T> | null;
  
    constructor() {
      this.head = null;
      this.tail = null;
    }
  
    append(data: T): void {
      const newNode = new Node(data);
      if (!this.head) {
        this.head = newNode;
        this.tail = newNode;
      } else {
        this.tail!.next = newNode;
        this.tail = newNode;
      }
    }
  
    toArray(): T[] {
      const result: T[] = [];
      let current = this.head;
      while (current) {
        result.push(current.data);
        current = current.next;
      }
      return result;
    }
  
    static fromArray<T>(array: T[]): LinkedList<T> {
      const list = new LinkedList<T>();
      array.forEach(item => list.append(item));
      return list;
    }
  }
  
  
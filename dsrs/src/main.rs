use dsrs::lists::LinkedList;

fn main() {
    let mut list = LinkedList::new();
    println!("{:?}", list);

    list.push_head(12);
    println!("{:?}", list);

    println!("{:?}", list.pop());
    println!("{:?}", list.pop());

    list.push_head(27);
    println!("{:?}", list);
    list.push_head(56);
    println!("{:?}", list);

    println!("{:?}", list.tail());
    list.push_tail(34);
    println!("{:?}", list.tail());
    println!("{:?}", list);

    while let Some(elem) = list.next() {
        println!("{:?}", elem)
    }
    println!("{:?}", list)
}

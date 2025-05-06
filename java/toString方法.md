# 重写toString的原因

在 Java 中，toString() 方法 是 Object 类的一个公共方法，所有 Java 对象都继承自 Object 类，因此每个类都有一个默认的 toString() 方法实现。默认的实现返回的是对象的类名加上它的哈希码（即内存地址），这种表示方式通常不适合我们实际需要。

1. 默认的 toString() 方法（继承自 Object）
```java
public class Person {
    String name;
    int age;

    Person(String name, int age) {
        this.name = name;
        this.age = age;
    }

    public static void main(String[] args) {
        Person p = new Person("Alice", 30);
        System.out.println(p);  // 输出：Person@5c647e05
    }
}
```
+ 这里，p 的输出结果是 Person@5c647e05，这是 Java 默认的 toString() 实现，它输出了类名（Person）和对象的哈希码（5c647e05，这个值是内存地址的某种表示）。

+ 这样的输出对于我们理解对象的实际内容没有任何帮助。

2. 重写 toString() 方法后
如果你想在输出 Person 对象时，看到更有意义的信息（例如 name 和 age），你需要重写 toString() 方法。

```java
public class Person {
    String name;
    int age;

    Person(String name, int age) {
        this.name = name;
        this.age = age;
    }

    @Override
    public String toString() {
        return "Person{name='" + name + "', age=" + age + '}';
    }

    public static void main(String[] args) {
        Person p = new Person("Alice", 30);
        System.out.println(p);  // 输出：Person{name='Alice', age=30}
    }
}
```

## toString()是被谁调用的

1.1 打印对象时（使用 System.out.println() 或 print()）
当你将一个对象传递给 System.out.println() 或 print() 方法时，Java 会自动调用该对象的 toString() 方法，将返回的字符串输出到控制台。

1.2 在字符串连接中（+ 运算符）
当你使用 + 运算符连接字符串时，如果一边是对象，Java 会自动调用该对象的 toString() 方法，将对象转换为字符串。

```java
Person p = new Person("Alice", 30);
String message = "Details: " + p;  // 自动调用 p.toString()
System.out.println(message);  // 输出：Details: Person{name='Alice', age=30}
```

这里，"Details: " + p 会调用 p.toString()，并将返回的字符串拼接到 "Details: " 后面。

1.3 集合类的输出（如 List、Set 等）
当你打印集合对象时，集合类会调用其元素的 toString() 方法来构建集合的字符串表示。

```java
List<Person> people = new ArrayList<>();
people.add(new Person("Alice", 30));
people.add(new Person("Bob", 25));

System.out.println(people);  // 输出：[Person{name='Alice', age=30}, Person{name='Bob', age=25}]
```

# JSON
使用variant 管理json数据类型 ，string_view进行代理模式明确只读语义，optional处理非侵入式异常处理机制。 通过递归下降对json字符串进行parser，解析后支持动态修改，并重新输出json格式，接口设计便捷，使用方便。
这段代码是一个JSON库，包括解析和生成JSON的功能。通过函数parser可以解析一个JSON字符串并返回一个Node对象，通过函数generate可以将一个Node对象转换为字符串形式的JSON。代码中使用了_variant和访问变种中的不同类型的值。最后，通过重载<<操作符，可以方便地将Node对象输出为JSON字符串。

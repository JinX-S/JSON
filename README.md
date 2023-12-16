# JSON
    这段代码是一个JSON库，包括解析和生成JSON的功能。optional处理非侵入式异常处理机制。通过函数parser可以解析一个JSON字符串并返回一个Node对象，
    通过函数generate可以将一个Node对象转换为字符串形式的JSON。代码中使用了_variant和访问变种中的不同类型的值。最后，通过重载<<操作符，可以方便地将Node对象输出为JSON字符串。

    程序先使用istream打开launch.json文件，通过stringstream把文件内容转为字符串，接着调用解析辅助函数auto parser(std::string_view json_str)对字符串开始进行处理，
    辅助函数将字符串传入JSON解析器类JsonParser进行处理，当遇到不同的字符时会进行不同的处理，当为'n'时解析'null'，当为't'时解析'true'，当为'f'解析'false'，当为'"'时解析字符串,
    当为'['时解析数组,当为'{'时解析对象,一般情况解析数字。解析完成后返回Node类型的JSON节点回主函数。这时可对其进行修改、插入、删除操作。完成之后就进行重新输出。
    已对'<<'进行重载，可用'<<'进行输出，使用'<<'进入JSON生成类，根据对应的类型生存对应的JSON。最后用输出流保存为新的.json文件。

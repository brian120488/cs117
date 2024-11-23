#// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
#//
#//                           PROXY FUNCTIONS
#//
#// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
def send_function_string(name):
    return f"""    string name = "{name}";
    RPCPROXYSOCKET->write(name.c_str(), name.length() + 1);\n"""
    
def proxy_function_header_string(name, info, decls):
    ret_type = info["return_type"]
    header = f"{ret_type} {name}("
    header_args = []
    for arg in info["arguments"]:
        arg_name = arg["name"]
        arg_type = arg["type"]
        types = decls["types"]
        type_of_type = types[arg_type]["type_of_type"]
        if type_of_type == "builtin":
            header_args.append(f"{arg_type} {arg_name}")
        elif type_of_type == "array":
            member_type = types[arg_type]["member_type"]
            element_count = types[arg_type]["element_count"]
            header_args.append(f"{member_type}[{element_count}] {arg_name}")
    header += ", ".join(header_args) + ") {"
    return header

def send_arg_string(i, arg):
    arg_name = arg["name"]
    arg_type = arg["type"]
    return f"""    char* arg{i} = reinterpret_cast<char*>(&{arg_name});
    RPCPROXYSOCKET->write(arg{i}, sizeof({arg_type}));\n"""
    
def read_output_string(info):
    ret_type = info["return_type"]
    return f"""    char readBuffer[sizeof({ret_type})];
    RPCPROXYSOCKET->read(readBuffer, sizeof({ret_type}));
    return *reinterpret_cast<{ret_type}*>(readBuffer);"""
    
    
#// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
#//
#//                           STUB FUNCTIONS
#//
#// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
def get_function_name_string():
    return """void getFunctionNameFromStream(char *buffer, unsigned bufSize) {
    char *bufp = buffer;     
    ssize_t readlen;  
    for (unsigned i = 0; i < bufSize; i++) {
        readlen = RPCSTUBSOCKET->read(bufp, 1);
        if (readlen == 0) break;
        if (*bufp++ == '\\0') break;
    }
}\n"""

def stub_function_header_string(name, info):
    args = ", ".join([arg["type"] + " " + arg["name"] for arg in info["arguments"]])
    return f"void __{name}({args})" + " {"

def get_output_string(name, info):
    ret_type = info["return_type"]
    args = ", ".join([arg["name"] for arg in info["arguments"]])
    return f"    {ret_type} output = {name}({args});"

def send_output_string(info):
    ret_type = info["return_type"]
    return f"""    char *outputBuffer = reinterpret_cast<char*>(&output);
    RPCSTUBSOCKET->write(outputBuffer, sizeof({ret_type}));"""


#// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
#//                          DISPATCH FUNCTIONS
#// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
def dispatch_function_string():
    return """void dispatchFunction() {
    char buffer[50];
    getFunctionNameFromStream(buffer, sizeof(buffer));
    string f_name(buffer);

    if (!RPCSTUBSOCKET-> eof()) {"""
    
def read_argument_string(i, arg):
    arg_name = arg["name"]
    arg_type = arg["type"]
    return f"""\t    char buffer{i}[sizeof({arg_type})];
    \t    RPCSTUBSOCKET->read(buffer{i}, sizeof({arg_type}));
    \t    {arg_type} {arg_name} = *reinterpret_cast<{arg_type}*>(buffer{i});\n"""

def dispatch_if_string(name, info):
    if_string = f'\tif (f_name == "{name}") ' + "{\n"
    args = ", ".join([arg["name"] for arg in info["arguments"]])
    for i, arg in enumerate(info["arguments"]):
        if_string += read_argument_string(i, arg) + "\n"
    if_string += f"\t    __{name} ({args});"
    if_string += "\n\t}\n"
    return if_string


# TODO: void functions, arrays, and structs, and nested structures
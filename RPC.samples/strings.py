#// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
#//
#//                           SUPPORT FUNCTIONS
#//
#// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
def sizeof(arg_type, decls):
    type_of_type = decls["types"][arg_type]["type_of_type"]
    
    if type_of_type == "builtin":
        return f"sizeof({arg_type})"
    elif type_of_type == "array":
        member_type = decls["types"][arg_type]["member_type"]
        element_count = decls["types"][arg_type]["element_count"]
        member_size = calculate_size(member_type, decls)
        return f"{member_size} * {element_count}"
    
def calculate_size(arg_type, decls):
    type_info = decls["types"][arg_type]
    type_of_type = type_info["type_of_type"]

    if type_of_type == "builtin":
        return f"sizeof({arg_type})"
    elif type_of_type == "array":
        member_type = type_info["member_type"]
        element_count = type_info["element_count"]
        member_size = calculate_size(member_type, decls)
        return f"{member_size} * {element_count}"
    else:
        raise ValueError(f"Unsupported type_of_type: {type_of_type}")

def get_param(arg_name, arg_type, decls, curr=""):
    types = decls["types"]
    type_of_type = types[arg_type]["type_of_type"]
    if type_of_type == "builtin":
        return f"{arg_type} {arg_name}{curr}"
    elif type_of_type == "array":
        member_type = types[arg_type]["member_type"]
        element_count = types[arg_type]["element_count"]
        return get_param(arg_name, member_type, decls, curr + f"[{element_count}]")
    
    
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
        param = get_param(arg_name, arg_type, decls)
        header_args.append(param)
    header += ", ".join(header_args) + ") {"
    return header

def send_arg_string(i, arg, decls):
    arg_name = arg["name"]
    arg_type = arg["type"]
    type_of_type = decls["types"][arg_type]["type_of_type"]
    ampersand = "&" if type_of_type != "array" else ""
    cast_line = f"char* arg{i} = reinterpret_cast<char*>({ampersand}{arg_name});"
    
    sizeof_arg = sizeof(arg_type, decls)
    write_line = f"RPCPROXYSOCKET->write(arg{i}, {sizeof_arg});"
    return f"""    {cast_line}\n    {write_line}\n"""
    
def read_output_string(info):
    ret_type = info["return_type"]
    if ret_type == "void": return ""
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

def stub_function_header_string(name, info, decls):
    header_args = []
    for arg in info["arguments"]:
        arg_name = arg["name"]
        arg_type = arg["type"]
        param = get_param(arg_name, arg_type, decls)
        header_args.append(param)
        
    header_args = ", ".join(header_args)
    return f"void __{name}({header_args})" + " {"

def get_output_string(name, info):
    ret_type = info["return_type"]
    args = ", ".join([arg["name"] for arg in info["arguments"]])
    if ret_type == "void": return f"    {name}({args});";
    return f"    {ret_type} output = {name}({args});"

def send_output_string(info):
    ret_type = info["return_type"]
    if ret_type == "void": return ""
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
    
def get_cast_type(arg_type, decls, placeholder):
    # TODO: make it work for get_cast_line()

def get_cast_line(arg, decls):
    arg_type = arg["type"]
    types = decls["types"]
    type_of_type = types[arg_type]["type_of_type"]
    if type_of_type == "builtin":
        return f"{arg_type} {arg_name} = *reinterpret_cast<{arg_type}*>(buffer{i});"
    elif type_of_type == "array":
        member_type = types[arg_type]["member_type"]
        element_count = types[arg_type]["element_count"]
        if types[member_type]["type_of_type"] == "builtin":
            return f"{member_type} *{arg_name} = reinterpret_cast<{member_type}(*)>(buffer{i});"
        
        lhs = get_cast_type(member_type, decls, f"*{arg_name}")
        cast_type = get_cast_type(member_type, decls, "*")
        return f"{lhs} = reinterpret_cast<{cast_type}>(buffer{i});"
            
    
def read_argument_string(i, arg, decls):
    arg_name = arg["name"]
    arg_type = arg["type"]
    types = decls["types"]
    type_of_type = types[arg_type]["type_of_type"]
    sizeof_arg = sizeof(arg_type, decls)
    
    declare_line = f"char buffer{i}[{sizeof_arg}];"
    read_line = f"RPCSTUBSOCKET->read(buffer{i}, {sizeof_arg});"
    
    if type_of_type == "builtin":
        cast_line = f"{arg_type} {arg_name} = *reinterpret_cast<{arg_type}*>(buffer{i});"
    elif type_of_type == "array":
        member_type = types[arg_type]["member_type"]
        element_count = types[arg_type]["element_count"]
        cast_line = f"{member_type} *{arg_name} = reinterpret_cast<{member_type}(*)>(buffer{i});"
    return f"\t    {declare_line}\n\t    {read_line}\n\t    {cast_line}\n"

def dispatch_if_string(name, info, decls):
    if_string = f'\tif (f_name == "{name}") ' + "{\n"
    args = ", ".join([arg["name"] for arg in info["arguments"]])
    for i, arg in enumerate(info["arguments"]):
        if_string += read_argument_string(i, arg, decls) + "\n"
    if_string += f"\t    __{name}({args});"
    if_string += "\n\t}\n"
    return if_string


# TODO: n-d arrays, structs, and nested structures
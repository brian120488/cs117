#// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
#//
#//                           SUPPORT FUNCTIONS
#//
#// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 

# get_param
# Retrieves the parameter associated with a given argument name and type from the declared functions.
# If the argument is a simple type, it returns the argument with its name.
# If the argument is an array or struct, it recurses into those types and returns the full type description.
def get_param(arg_name, arg_type, decls, curr=""):
    types = decls["types"]
    type_of_type = types[arg_type]["type_of_type"]
    if type_of_type == "builtin" or type_of_type == "struct":
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
# send_function_string
# Generates the code that sends a function name to the stub 
def send_function_string(name):
    return f"""    string name = "{name}";
    *GRADING << \"Sending function name {name} to stub.\" << endl;
    RPCPROXYSOCKET->write(name.c_str(), name.length() + 1);\n"""

# proxy_function_header_string
# Generates the header for a proxy function, including return type and parameters,
# based on the function declaration.
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

# send_arg_string
# Generates code that sends an argument to a stub, handling different types such as built-in types, arrays, and structs.
# The function recurses for arrays and structs to correctly send each element/member.
def send_arg_string(arg_name, arg_type, decls, curr="", i="i", last_type=""):
    type_info = decls["types"][arg_type]
    type_of_type = type_info["type_of_type"]
    
    if type_of_type == "builtin":
        if last_type == "": # if main arg
            return f"""    *GRADING << \"Sending {arg_name} to stub.\" << endl;
    write_{arg_type}({arg_name});"""
        elif arg_name == "": # if in array
            return f"    write_{arg_type}({curr});" 
        else: # if in struct
            return f"    write_{arg_type}({curr}.{arg_name});" 

    elif type_of_type == "array":
        ret = []
        if last_type == "": # if main arg
            ret.append(f"    *GRADING << \"Sending {arg_name} to stub.\" << endl;")
        
        member_type = type_info["member_type"]
        element_count = type_info["element_count"]
        ret.append(f"    for (int {i} = 0; {i} < {element_count}; {i}++) " + "{")
        new_curr = f"{curr}.{arg_name}[{i}]" if last_type == "struct" else f"{curr}{arg_name}[{i}]"
        ret.append("        " + send_arg_string("", member_type, decls, new_curr, chr(ord(i) + 1), "array"))
        ret.append("    }")
        return "\n".join(ret)
    elif type_of_type == "struct":
        ret = []
        if last_type == "": # if main arg
            ret.append(f"    *GRADING << \"Sending {arg_name} to stub.\" << endl;")
        for member in type_info["members"]:
            member_name = member["name"]
            member_type = member["type"]
            new_curr = f"{curr}.{arg_name}" if last_type == "struct" else f"{curr}{arg_name}"
            ret.append(send_arg_string(member_name, member_type, decls, new_curr, i, "struct"))
        return "\n".join(ret)

# read_struct
# Reads the contents of a struct argument from the proxy, handling different member types.
# The function recurses for arrays and structs to correctly read each element/member.
def read_struct(arg_type, decls, curr="", i="i"):
    type_info = decls["types"][arg_type]
    type_of_type = type_info["type_of_type"]
    
    if type_of_type == "builtin":
        return f"    {curr} = read_{arg_type}();"
    elif type_of_type == "array":
        ret = []
        member_type = type_info["member_type"]
        element_count = type_info["element_count"]
        ret.append(f"    for (int {i} = 0; {i} < {element_count}; {i}++) " + "{")
        ret.append("        " + read_struct(member_type, decls, f"{curr}[{i}]", chr(ord(i) + 1)))
        ret.append("    }")
        return "\n".join(ret)
    elif type_of_type == "struct":
        ret = []
        for member in type_info["members"]:
            member_name = member["name"]
            member_type = member["type"]
            new_curr = f"{curr}.{member_name}"
            ret.append(read_struct(member_type, decls, new_curr))
        return "\n".join(ret)

# read_output_string
# Generates the code to read the output of a function call from the stub and return the result.
# If the return type is void, it returns nothing. For structs or built-in types, it reads and returns the result. 
# It calls read_struct to handle more complicated structs 
def read_output_string(info, decls):
    ret_type = info["return_type"]
    if ret_type == "void": return ""
    type_of_type = decls["types"][ret_type]["type_of_type"]
   
    ret = []
    if type_of_type == "builtin":
        ret.append(f"\n    {ret_type} result = read_{ret_type}();")
    elif type_of_type == "struct":
        ret.append(f"\n    {ret_type} result;")
        ret.append(read_struct(ret_type, decls, curr="result"))
    ret.append("    *GRADING << \"Received result.\" << endl;")
    ret.append("    return result;")
    return "\n".join(ret)

    
    
#// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
#//
#//                           STUB FUNCTIONS
#//
#// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
# get_function_name_string
# Returns the function string to retrieve the function name from the proxy in the stub.
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

# stub_function_header_string
# Generates the header for a stub function, including return type and parameters, 
# based on function declaration.
def stub_function_header_string(name, info, decls):
    header_args = []
    for arg in info["arguments"]:
        arg_name = arg["name"]
        arg_type = arg["type"]
        param = get_param(arg_name, arg_type, decls)
        header_args.append(param)
    
    header_args = ", ".join(header_args)
    return f"void __{name}({header_args})" + " {"

# get_output_string
# Generates code that calls the function and processes the output, handling return values and logging.
def get_output_string(name, info):
    ret_type = info["return_type"]
    args = ", ".join([arg["name"] for arg in info["arguments"]])
    if ret_type == "void": 
        return f"""    *GRADING << \"Running {name}({args}).\" << endl;
    {name}({args});"""
    
    return f"""    *GRADING << \"Running {name}({args}).\" << endl;
    {ret_type} output = {name}({args});"""

# write_struct
# Generates code that writes a struct's contents to the proxy, handling different member types.
# The function recurses for arrays and structs to correctly write each element/member.
def write_struct(arg_type, decls, curr="", i="i"):
    type_info = decls["types"][arg_type]
    type_of_type = type_info["type_of_type"]
    
    if type_of_type == "builtin":
        return f"    write_{arg_type}({curr});"
    elif type_of_type == "array":
        ret = []
        member_type = type_info["member_type"]
        element_count = type_info["element_count"]
        ret.append(f"    for (int {i} = 0; {i} < {element_count}; {i}++) " + "{")
        ret.append("        " + write_struct(member_type, decls, f"{curr}[{i}]", chr(ord(i) + 1)))
        ret.append("    }")
        return "\n".join(ret)
    elif type_of_type == "struct":
        ret = []
        for member in type_info["members"]:
            member_name = member["name"]
            member_type = member["type"]
            new_curr = f"{curr}.{member_name}"
            ret.append(write_struct(member_type, decls, new_curr))
        return "\n".join(ret)

# send_output_string
# Generates code to send the output of a function to the proxy, handling return 
# values for built-in types and structs. 
# Calls write_struct to handle complex structs to write to proxy
def send_output_string(info, decls):
    ret_type = info["return_type"]
    if ret_type == "void": return ""
    type_info = decls["types"][ret_type]
    type_of_type = type_info["type_of_type"]
   
    ret = []
    ret.append("    *GRADING << \"Sending output to proxy.\" << endl;")
    if type_of_type == "builtin":
        ret.append(f"\n    write_{ret_type}(output);")
    elif type_of_type == "struct":
        ret.append(write_struct(ret_type, decls, curr="output"))
    return "\n".join(ret)


#// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
#//                          DISPATCH FUNCTIONS
#// - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
# dispatch_function_string
# Generates code that dispatches a function based on the function name read from the proxy, for RPC handling.
def dispatch_function_string():
    return """void dispatchFunction() {
    char buffer[50];
    getFunctionNameFromStream(buffer, sizeof(buffer));
    string f_name(buffer);

    if (!RPCSTUBSOCKET-> eof()) {
        *GRADING << "Read function name " << f_name << "." << endl;"""

# get_cast_type
# Retrieves the type cast required for an argument type, considering array and struct types.
# This function is used to format arguments for sending/reading when dealing with 
# complex types like arrays and structs.
def get_cast_type(arg_type, decls, placeholder):
    type_info = decls["types"][arg_type]
    type_of_type = type_info["type_of_type"]

    if type_of_type == "array":
        member_type = type_info["member_type"]
        element_count = type_info["element_count"]
        inner_cast = get_cast_type(member_type, decls, placeholder)
        return f"[{element_count}]{inner_cast}"
    else:
        return placeholder

# get_member_type
# Retrieves the member type of an argument, handling arrays and struct types.
# This is used to recursively get the type of array members or struct fields.
def get_member_type(arg_type, decls):
    type_info = decls["types"][arg_type]
    type_of_type = type_info["type_of_type"]

    if type_of_type == "array":
        member_type = type_info["member_type"]
        return get_member_type(member_type, decls)
    elif type_of_type == "builtin" or type_of_type == "struct":
        return arg_type

# read_arg_string
# Generates code to read an argument from the proxy, handling different types (built-in, arrays, structs).
# It recurses for arrays and structs, ensuring that all nested elements are correctly read.
def read_arg_string(arg_name, arg_type, decls, curr="", i="i", last_type=""):
    types = decls["types"]
    type_info = types[arg_type]
    type_of_type = type_info["type_of_type"]
    
    ret = []
    if last_type == "": # if main arg
        grade_log = f"            *GRADING << \"Reading {arg_name} from proxy.\" << endl;"
        ret.append(grade_log)
        
    if type_of_type == "builtin":  
        if last_type == "": # if main arg
            read_line = f"        {arg_type} {arg_name} = read_{arg_type}();"
        elif arg_name != "": # if in struct
            curr += "." + arg_name 
            read_line = f"        {curr} = read_{arg_type}();"
        elif arg_name == "": # if in array
            read_line = f"        {arg_name}{curr} = read_{arg_type}();" 
        ret.append(read_line)
    elif type_of_type == "array":
        read_line = []
        member_type = type_info["member_type"]
        element_count = type_info["element_count"]
        if last_type == "": # if main arg
            ele_type = get_member_type(arg_type, decls)
            cast_type = get_cast_type(arg_type, decls, "")
            read_line.append(f"            {ele_type} {arg_name}{cast_type};")
            
        read_line.append(f"        for (int {i} = 0; {i} < {element_count}; {i}++) " + "{")   
        new_curr = f"{curr}.{arg_name}[{i}]" if last_type == "struct" else f"{curr}{arg_name}[{i}]"
        read_line.append("        " + read_arg_string("", member_type, decls, new_curr, chr(ord(i) + 1), "array"))
        
        read_line.append("            }")
        read_line = "\n".join(read_line)
        ret.append(read_line)
    elif type_of_type == "struct":
        if last_type == "": # if main arg
            ret.append(f"{arg_type} {arg_name};")
        for member in type_info["members"]:
            member_name = member["name"]
            member_type = member["type"]
            new_curr = f"{curr}.{arg_name}" if last_type == "struct" else f"{curr}{arg_name}"
            ret.append(read_arg_string(member_name, member_type, decls, new_curr, i, "struct"))
    return "\n".join(ret)

# dispatch_if_string
# Generates a C++ `if` block string to dispatch a function call based on a given name.
def dispatch_if_string(name, info, decls):
    if_string = f'\tif (f_name == "{name}") ' + "{\n"
    args = ", ".join([arg["name"] for arg in info["arguments"]])
    for i, arg in enumerate(info["arguments"]):
        if_string += read_arg_string(arg["name"], arg["type"], decls) + "\n"
    if_string += f"\n\t    __{name}({args});"
    if_string += "\n\t}\n"
    return if_string

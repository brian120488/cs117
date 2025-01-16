def proxy_helpers():
    return """
void write_int(int n) {
    char buffer[4];
    n = htonl(n);
    memcpy(buffer, &n, 4);
    RPCPROXYSOCKET->write(buffer, 4);
}

int read_int() {
    char buffer[4];
    RPCPROXYSOCKET->read(buffer, 4);
    int n;
    memcpy(&n, buffer, 4);
    return ntohl(n);
}

void write_float(float n) {
    char buffer[4];
    int tmp;
    memcpy(&tmp, &n, 4);
    tmp = htonl(tmp);
    memcpy(buffer, &tmp, 4);
    RPCPROXYSOCKET->write(buffer, 4);
}

float read_float() {
    char buffer[4];
    RPCPROXYSOCKET->read(buffer, 4);
    float n;
    int tmp;
    memcpy(&tmp, buffer, 4);
    tmp = ntohl(tmp);
    memcpy(&n, &tmp, 4);
    return n;
}

void write_string(string s) {
    write_int(s.length());
    RPCPROXYSOCKET->write(s.c_str(), s.length() + 1);
}

string read_string() {
    int length = read_int();
    char buffer[length + 1]; 
    RPCPROXYSOCKET->read(buffer, length + 1); 
    string s(buffer);
    return s;
}"""


def stub_helpers():
    return """
void write_int(int n) {
    char buffer[4];
    n = htonl(n);
    memcpy(buffer, &n, 4);
    RPCSTUBSOCKET->write(buffer, 4);
}

int read_int() {
    char buffer[4];
    RPCSTUBSOCKET->read(buffer, 4);
    int n;
    memcpy(&n, buffer, 4);
    return ntohl(n);
}

void write_float(float n) {
    char buffer[4];
    int tmp;
    memcpy(&tmp, &n, 4);
    tmp = htonl(tmp);
    memcpy(buffer, &tmp, 4);
    RPCSTUBSOCKET->write(buffer, 4);
}

float read_float() {
    char buffer[4];
    RPCSTUBSOCKET->read(buffer, 4);
    float n;
    int tmp;
    memcpy(&tmp, buffer, 4);
    tmp = ntohl(tmp);
    memcpy(&n, &tmp, 4);
    return n;
}

void write_string(string s) {
    write_int(s.length());
    RPCSTUBSOCKET->write(s.c_str(), s.length() + 1);
}

string read_string() {
    int length = read_int();
    // cout << length << endl;
    char buffer[length + 1]; 
    RPCSTUBSOCKET->read(buffer, length + 1); 
    string s(buffer);
    return s;
}"""
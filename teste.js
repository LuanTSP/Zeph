class Token {
  constructor(type, value) {
    this.type = type;
    this.value = value;
  }
}

const precedence = {
  "+": 0,
  "*": 1,
  "/": 2,
}

const tokens = [
  new Token("NUMBER", 1),
  new Token("BINARY_OP", "/"),
  new Token("NUMBER", 2), 
  new Token("BINARY_OP", "*"), 
  new Token("NUMBER", 3), 
  new  Token("BINARY_OP", "+"), 
  new Token("NUMBER", 4), 
  new Token("EOF", "eof")
];

function peak() {
  return tokens[0];
}

function advance() {
  return tokens.shift();
}

function parsePrimary() {
  const token = peak();
  if (token.type != "NUMBER") {
    throw new Error("Exprected number")
  }
  advance();
  return { type: "LITERAL", value: token.value } 
}

function parseExpr(minPrec = -1) {
  let left = parsePrimary();
  // 1 / 2 * 3 + 4
  while (peak().type != "EOF" && peak().type == "BINARY_OP") {
    const token = peak();

    const prec = precedence[token.value];
    if (prec < minPrec) {
      break;
    }

    advance();

    const right = parseExpr(prec + 1);
    
    left = {
      type: "BINARY_OPERATION",
      operator: token.value, left, right
    }
  }
  
  return left;
}

console.log(parseExpr());



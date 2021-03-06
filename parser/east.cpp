#include "east.h"

//astParser
east::astParser::astParser(std::vector<epplex::Token> tg_) : tg(tg_) {}

epplex::Token* east::astParser::peek(int pos_){
    return new epplex::Token(this->tg[this->pos + pos_]);
}

epplex::Token* east::astParser::token(){
    return new epplex::Token(this->tg[this->pos++]);
}

east::WholeExprNode* east::astParser::gen_wholeExprNode(){
    if(east::WholeExprNode::is_it(*this)){
        east::WholeExprNode* node = new east::WholeExprNode;
        if(east::AssignExprNode::is_it(*this)) node->assignexpr = gen_assignExprNode();
        else if(east::ValExprNode::is_it(*this)) node->valexpr = gen_valExprNode();
        return node;
    }
    else throw epperr::Epperr("SyntaxErrror", "Unknown type of the expr!", tg[pos].line, tg[pos].column);
}
east::ValExprNode* east::astParser::gen_valExprNode(){
    east::ValExprNode* node = new east::ValExprNode;
    if(east::ListExprNode::is_it(*this))
        node->listexpr = gen_listExprNode();
    else if(east::BoolExprNode::is_it(*this))
        node->boolexpr = gen_boolExprNode();
    else if(east::FuncDefineExprNode::is_it(*this))
        node->fdefexpr = gen_fdefExprNode();
    else
        node->addexpr = gen_addExprNode();
    return node;
}
east::AddOpNode* east::astParser::gen_addOpNode(){
    if(east::AddOpNode::is_it(*this)){
        east::AddOpNode* node = new east::AddOpNode;
        node->op = token();
        return node;
    }
    else throw epperr::Epperr("SyntaxError", "Expect '+' or '-'!", tg[pos].line, tg[pos].column);
}
east::MulOpNode* east::astParser::gen_mulOpNode(){
    if(east::MulOpNode::is_it(*this)){
        east::MulOpNode* node = new east::MulOpNode;
        node->op = token();
        return node;
    }
    else throw epperr::Epperr("SyntaxError", "Expect '/', '*' or '%'!", tg[pos].line, tg[pos].column);
}
east::CmpOpNode* east::astParser::gen_cmpOpNode(){
    if(east::CmpOpNode::is_it(*this)){
        east::CmpOpNode* node = new east::CmpOpNode;
        node->op = token();
        return node;
    }
    else throw epperr::Epperr("SyntaxError", "Expect '==', '!=', '>', '<', '>=' or '<='!", tg[pos].line, tg[pos].column);
}
east::BoolOpNode* east::astParser::gen_boolOpNode(){
    if(east::BoolOpNode::is_it(*this)){
        east::BoolOpNode* node = new east::BoolOpNode;
        node->op = token();
        return node;
    }
    else throw epperr::Epperr("SyntaxError", "Expect '||' or '&&'!", tg[pos].line, tg[pos].column);
}
east::AssignExprNode* east::astParser::gen_assignExprNode(){
    if(east::AssignExprNode::is_it(*this)){
        east::AssignExprNode* node = new east::AssignExprNode;
        node->iden = gen_identifierNode();
        if(peek()->content == "=") node->equ = token();
        else throw epperr::Epperr("SyntaxError", "Expect '='!", tg[pos].line, tg[pos].column);
        node->val = gen_valExprNode();
        if(peek()->content == ";") node->end = token();
        else throw epperr::Epperr("SyntaxError", "Expect ';'", tg[pos].line, tg[pos].column);
        return node;
    }
    else throw epperr::Epperr("SyntaxError", "It is not a proper Assign statement format", tg[pos].line, tg[pos].column);
}
east::PrimExprNode* east::astParser::gen_primExprNode(){
    if(east::PrimExprNode::is_it(*this)){
        east::PrimExprNode* node = new east::PrimExprNode;
        if(east::FuncCallExprNode::is_it(*this)) node->fcall = gen_fcallExprNode();
        else if(east::SelfIaDExprNode::is_it(*this)) node->siad = gen_siadExprNode();
        else if(east::PrintoLnExprNode::is_it(*this)) node->poln = gen_polnExprNode();
        else if(east::TypeOfExprNode::is_it(*this)) node->tpof = gen_tpofExprNode();
        else if(east::InputExprNode::is_it(*this)) node->input = gen_inputExprNode();
        else if(east::TypeToExprNode::is_it(*this)) node->typeto = gen_tytExprNode();
        else if(east::LenExprNode::is_it(*this)) node->glen = gen_lenExprNode();
        else if(peek()->type == "__IDENTIFIER__") node->iden = gen_identifierNode();
        else if(peek()->type == "__NUMBER__") node->number = token();
        else if(peek()->type == "__STRING__") node->str = token();
        else if(peek()->type == "__CHAR__") node->ch = token();
        else if(peek()->content == "true" || peek()->content == "false") node->boolconst = token();
        else if(peek()->content == "(") {
            node->left = token();
            if(east::AddExprNode::is_it(*this)){
                node->addexpr = gen_addExprNode();
            }
            else if(east::BoolExprNode::is_it(*this)){
                node->boolexpr = gen_boolExprNode();
            }
            else throw epperr::Epperr("SyntaxErrror", "Unknown type of the expr!!", tg[pos].line, tg[pos].column);
            if(peek()->content == ")") node->right = token();
            else throw epperr::Epperr("SyntaxErrror", "Expect ')'!", tg[pos].line, tg[pos].column);
        }
        return node;
    }
    else throw epperr::Epperr("SyntaxErrror", "Unknown type of the expr!", tg[pos].line, tg[pos].column);
}
east::SelfIaDExprNode* east::astParser::gen_siadExprNode(){
    if(east::SelfIaDExprNode::is_it(*this)){
        east::SelfIaDExprNode* node = new east::SelfIaDExprNode;
        if(peek()->content == "++" || peek()->content == "--") {
            node->op = token();
            node->isFront = true;
        }
        else node->iden = gen_identifierNode();
        if(node->isFront == true && peek()->type == "__IDENTIFIER__") node->iden = gen_identifierNode();
        else if(peek()->content == "++" || peek()->content == "--") node->op = token();
        else throw epperr::Epperr("SyntaxErrror", "Expect '++', '--' or an identifier!", tg[pos].line, tg[pos].column);;
        return node;
    }
    else throw epperr::Epperr("SyntaxErrror", "Unknown type of the expr!", tg[pos].line, tg[pos].column);
}
east::FuncCallExprNode* east::astParser::gen_fcallExprNode(){
    if(east::FuncCallExprNode::is_it(*this)){
        east::FuncCallExprNode* node = new east::FuncCallExprNode;
        node->func_name = gen_identifierNode();token();
        if(east::ValExprNode::is_it(*this)) node->act_paras.push_back(gen_valExprNode());
        while(true){
            if(peek()->content != ",") break;
            token();
            node->act_paras.push_back(gen_valExprNode());
        }
        if(peek()->content == ")") token();
        else throw epperr::Epperr("SyntaxErrror", "Expect ')'!", tg[pos].line, tg[pos].column);
        return node;
    }
    else throw epperr::Epperr("SyntaxErrror", "Unknown type of the expr!", tg[pos].line, tg[pos].column);
}
east::InputExprNode* east::astParser::gen_inputExprNode(){
    if(east::InputExprNode::is_it(*this)){
        east::InputExprNode* node = new east::InputExprNode;
        node->mark = token();
        if(peek()->content == "(") token();
        else throw epperr::Epperr("SyntaxErrror", "Expect '('!", tg[pos].line, tg[pos].column);
        if(east::ValExprNode::is_it(*this)) node->expr = gen_valExprNode();
        else ;
        if(peek()->content == ")") token();
        else throw epperr::Epperr("SyntaxErrror", "Expect ')'!", tg[pos].line, tg[pos].column);
        return node;
    }
    else throw epperr::Epperr("SyntaxErrror", "Unknown type of the expr!", tg[pos].line, tg[pos].column);
}
east::TypeOfExprNode* east::astParser::gen_tpofExprNode(){
    if(east::TypeOfExprNode::is_it(*this)){
        east::TypeOfExprNode* node = new east::TypeOfExprNode;
        node->mark = token();
        if(peek()->content == "(") token();
        else throw epperr::Epperr("SyntaxErrror", "Expect '('!", tg[pos].line, tg[pos].column);
        if(east::ValExprNode::is_it(*this)) node->expr = gen_valExprNode();
        else throw epperr::Epperr("SyntaxErrror", "Unknown type of the expr!", tg[pos].line, tg[pos].column);
        if(peek()->content == ")") token();
        else throw epperr::Epperr("SyntaxErrror", "Expect ')'!", tg[pos].line, tg[pos].column);
        return node;
    }
    else throw epperr::Epperr("SyntaxErrror", "Unknown type of the expr!", tg[pos].line, tg[pos].column);
}
east::LenExprNode* east::astParser::gen_lenExprNode(){
    if(east::LenExprNode::is_it(*this)){
        east::LenExprNode* node = new east::LenExprNode;
        node->mark = token();
        if(peek()->content == "(") token();
        else throw epperr::Epperr("SyntaxErrror", "Expect '('!", tg[pos].line, tg[pos].column);
        if(east::ValExprNode::is_it(*this)) node->expr = gen_valExprNode();
        else throw epperr::Epperr("SyntaxErrror", "Unknown type of the expr!", tg[pos].line, tg[pos].column);
        if(peek()->content == ")") token();
        else throw epperr::Epperr("SyntaxErrror", "Expect ')'!", tg[pos].line, tg[pos].column);
        return node;
    }
    else throw epperr::Epperr("SyntaxErrror", "Unknown type of the expr!", tg[pos].line, tg[pos].column);
}
east::PrintoLnExprNode* east::astParser::gen_polnExprNode(){
    if(east::PrintoLnExprNode::is_it(*this)){
        east::PrintoLnExprNode* node = new east::PrintoLnExprNode;
        node->mark = token();
        if(peek()->content == "(") token();
        else throw epperr::Epperr("SyntaxErrror", "Expect '('!", tg[pos].line, tg[pos].column);
        if(east::ValExprNode::is_it(*this)) node->expr = gen_valExprNode();
        else throw epperr::Epperr("SyntaxErrror", "Unknown type of the expr!", tg[pos].line, tg[pos].column);
        if(peek()->content == ")") token();
        else throw epperr::Epperr("SyntaxErrror", "Expect ')'!", tg[pos].line, tg[pos].column);
        return node;
    }
    else throw epperr::Epperr("SyntaxErrror", "Unknown type of the expr!", tg[pos].line, tg[pos].column);
}
east::TypeToExprNode* east::astParser::gen_tytExprNode(){
    if(east::TypeToExprNode::is_it(*this)){
        east::TypeToExprNode* node = new east::TypeToExprNode;
        node->mark = token();
        if(peek()->content == "(") token();
        else throw epperr::Epperr("SyntaxErrror", "Expect '('!", tg[pos].line, tg[pos].column);
        if(east::ValExprNode::is_it(*this)) node->expr = gen_valExprNode();
        else throw epperr::Epperr("SyntaxErrror", "Unknown type of the expr!", tg[pos].line, tg[pos].column);
        if(peek()->content == ")") token();
        else throw epperr::Epperr("SyntaxErrror", "Expect ')'!", tg[pos].line, tg[pos].column);
        return node;
    }
    else throw epperr::Epperr("SyntaxErrror", "Unknown type of the expr!", tg[pos].line, tg[pos].column);
}
east::IdentifierNode* east::astParser::gen_identifierNode(){
    if(east::IdentifierNode::is_it(*this)){
        east::IdentifierNode* node = new east::IdentifierNode;
        if(peek(1)->content == "["){
            east::IdentifierNode* n = new east::IdentifierNode("__ARRE__");
            n->idens.push_back(token());
            n->arrleft = token();
            if(east::AddExprNode::is_it(*this)) n->arrindex = gen_addExprNode();
            else throw epperr::Epperr("SyntaxError", "This type of data cannot be used as an index to an array!", tg[pos].line, tg[pos].column);
            if(peek()->content == "]") n->arrright = token();
            else throw epperr::Epperr("SyntaxErrror", "Expect ']'!", tg[pos].line, tg[pos].column);
            node = n;
        }
        else{
            east::IdentifierNode* n = new east::IdentifierNode("__PURE__");
            n->idens.push_back(token());
            while(true){
                if(peek()->content != ".") break;
                n->dots.push_back(token());
                n->idens.push_back(token());
            }
            node = n;
        }
        return node;
    }
    else throw epperr::Epperr("SyntaxErrror", "Unknown type of the expr!", tg[pos].line, tg[pos].column);
}
east::AddExprNode* east::astParser::gen_addExprNode(){
    if(east::AddExprNode::is_it(*this)){
        east::AddExprNode* node = new east::AddExprNode;
        node->muls.push_back(gen_mulExprNode());
        while(true){
            if(!east::AddOpNode::is_it(*this)) break;
            node->ops.push_back(gen_addOpNode());
            node->muls.push_back(gen_mulExprNode());
        }
        return node;
    }
    else throw epperr::Epperr("SyntaxError", "Unknown type of the expr!", tg[pos].line, tg[pos].column);
}
east::MulExprNode* east::astParser::gen_mulExprNode(){
    if(east::MulExprNode::is_it(*this)){
        east::MulExprNode* node = new east::MulExprNode;
        node->prims.push_back(gen_primExprNode());
        while(true){
            if(!east::MulOpNode::is_it(*this)) break;
            node->ops.push_back(gen_mulOpNode());
            node->prims.push_back(gen_primExprNode());
        }
        return node;
    }
    else throw epperr::Epperr("SyntaxError", "Unknown type of the expr!", tg[pos].line, tg[pos].column);
}
east::CmpExprNode* east::astParser::gen_cmpExprNode(){
    if(east::CmpExprNode::is_it(*this)){
        east::CmpExprNode* node = new east::CmpExprNode;
        node->expr = gen_addExprNode();
        if(east::CmpOpNode::is_it(*this)) node->op = gen_cmpOpNode();
        if(east::AddExprNode::is_it((*this))) node->target = gen_addExprNode();
        return node;
    }
    else throw epperr::Epperr("SyntaxError", "Unknown type of the expr!", tg[pos].line, tg[pos].column);
}
east::BoolExprNode* east::astParser::gen_boolExprNode(){
    if(east::BoolExprNode::is_it(*this)){
        east::BoolExprNode* node = new east::BoolExprNode;
        if(peek()->content == "!") node->notsign = token();
        node->cmps.push_back(gen_cmpExprNode());
        while(true){
            if(!east::BoolOpNode::is_it(*this)) break;
            node->ops.push_back(gen_boolOpNode());
            node->cmps.push_back(gen_cmpExprNode());
        }
        return node;
    }
    else throw epperr::Epperr("SyntaxError", "Unknown type of the expr!", tg[pos].line, tg[pos].column);
}
east::ListExprNode* east::astParser::gen_listExprNode(){
    if(east::ListExprNode::is_it(*this)){
        east::ListExprNode* node = new east::ListExprNode;
        node->left = token();
        node->arrayelts.push_back(gen_valExprNode());
        while(true){
            if(peek()->content != ",") break;
            token();
            node->arrayelts.push_back(gen_valExprNode());
        }
        if(peek()->content == "]") node->right = token();
        else throw epperr::Epperr("SyntaxError", "Expect ']'!", tg[pos].line, tg[pos].column);
        return node;
    }
    else throw epperr::Epperr("SyntaxError", "Expect '['!", tg[pos].line, tg[pos].column);
}
east::FuncDefineExprNode* east::astParser::gen_fdefExprNode(){
    if(east::FuncDefineExprNode::is_it(*this)){
        east::FuncDefineExprNode* node = new east::FuncDefineExprNode;
        node->mark = token();
        if(peek()->content == "(") node->left = token();
        else throw epperr::Epperr("SyntaxError", "Expect '('!", tg[pos].line, tg[pos].column);
        if(east::TypeToExprNode::is_it(*this)) node->paras.push_back(gen_tytExprNode());
        while(true){
            if(peek()->content != ",") break;
            token();
            node->paras.push_back(gen_tytExprNode());
        }
        if(peek()->content == ")") node->right = token();
        else throw epperr::Epperr("SyntaxError", "Expect ')'!", tg[pos].line, tg[pos].column);
        if(east::BlockStmtNode::is_it(*this)) node->body = gen_blockStmtNode();
        else throw epperr::Epperr("SyntaxError", "A function needs a function body when it is defined!", tg[pos].line, tg[pos].column);
        return node;
    }
    else throw epperr::Epperr("SyntaxError", "Unknown type of the expr!", tg[pos].line, tg[pos].column);
}

//stmt
east::StatNode* east::astParser::gen_statNode(){
    if(east::StatNode::is_it(*this)){
        east::StatNode* node = new east::StatNode;
        while(east::StmtNode::is_it(*this)){
            node->stmts.emplace_back(gen_stmtNode());
        }
        return node;
    }
    else throw epperr::Epperr("SyntaxError", "Unknown type of the stmt!", tg[pos].line, tg[pos].column);
}
east::StmtNode* east::astParser::gen_stmtNode(){
    if(east::StmtNode::is_it(*this)){
        east::StmtNode* node = new east::StmtNode;
        if(east::OutStmtNode::is_it(*this)) node->outstmt = gen_outStmtNode();
        else if(east::VorcStmtNode::is_it(*this)) node->vorcstmt = gen_vorcStmtNode();
        else if(east::AssignStmtNode::is_it(*this)) node->assignstmt = gen_assignStmtNode();
        else if(east::DeleteStmtNode::is_it(*this)) node->deletestmt = gen_delStmtNode();
        else if(east::BlockStmtNode::is_it(*this)) node->blockstmt = gen_blockStmtNode();
        else if(east::IfStmtNode::is_it(*this)) node->ifstmt = gen_ifStmtNode();
        else if(east::ElseifStmtNode::is_it(*this)) node->elifstmt = gen_elifStmtNode();
        else if(east::ElseStmtNode::is_it(*this)) node->elsestmt = gen_elseStmtNode();
        else if(east::BreakStmtNode::is_it(*this)) node->brkstmt = gen_brkStmtNode();
        else if(east::WhileStmtNode::is_it(*this)) node->whilestmt = gen_whileStmtNode();
        else if(east::RepeatStmtNode::is_it(*this)) node->reptstmt = gen_reptStmtNode();
        else if(east::ForEachStmtNode::is_it(*this)) node->foreachstmt = gen_foreachStmtNode();
        else if(east::AreaStmtNode::is_it(*this)) node->areastmt = gen_areaStmtNode();
        return node;
    }
    else throw epperr::Epperr("SyntaxError", "Unknown type of the stmt!", tg[pos].line, tg[pos].column);
}
east::OutStmtNode* east::astParser::gen_outStmtNode(){
    if(east::OutStmtNode::is_it(*this)){
        east::OutStmtNode* node = new east::OutStmtNode;
        node->mark = token();
        if(east::ValExprNode::is_it(*this)) node->content = gen_valExprNode();
        else throw epperr::Epperr("SyntaxError", "Requires an expression", tg[pos].line, tg[pos].column);

        if(peek()->content == ";") node->end = token();
        else throw epperr::Epperr("SyntaxError", "Expect ';'", tg[pos].line, tg[pos].column);

        return node;
    }
    else throw epperr::Epperr("SyntaxError", "It is not a proper Out statement format", tg[pos].line, tg[pos].column);
}
east::VorcStmtNode* east::astParser::gen_vorcStmtNode(){
    if(east::VorcStmtNode::is_it(*this)){
        east::VorcStmtNode* node = new east::VorcStmtNode;
        bool ready_type = false;
        node->mark = token();
        if(peek()->type == "__IDENTIFIER__") node->iden = token();
        else throw epperr::Epperr("SyntaxError", "When defining a variable or constant, a name must be specified", tg[pos].line, tg[pos].column);
        if(peek()->content == ":") {node->type_exp = token(); ready_type = true;}
        if(peek()->content == "int" || peek()->content == "deci" || peek()->content == "string" || peek()->content == "char" || peek()->content == "bool" && ready_type) node->type = token();
        else if(peek()->content == "int" || peek()->content == "deci" || peek()->content == "string" || peek()->content == "char" || peek()->content == "bool" && !ready_type)
            throw epperr::Epperr("SyntaxError", "Type declarators cannot be used without a type description", tg[pos].line, tg[pos].column);
        if(peek()->content == "=") node->equ = token();
        else throw epperr::Epperr("SyntaxError", "Expect '='", tg[pos].line, tg[pos].column);
        node->value = gen_valExprNode();
        if(peek()->content == ";") node->end = token();
        else throw epperr::Epperr("SyntaxError", "Expect ';'", tg[pos].line, tg[pos].column);
        return node;
    }
    else throw epperr::Epperr("SyntaxError", "It is not a proper Define-variable or Define-constant statement format", tg[pos].line, tg[pos].column);
}
east::AssignStmtNode* east::astParser::gen_assignStmtNode(){
    if(east::AssignStmtNode::is_it(*this)){
        east::AssignStmtNode* node = new east::AssignStmtNode;
        node->iden = gen_identifierNode();
        if(peek()->content == "=") node->equ = token();
        else throw epperr::Epperr("SyntaxError", "Expect '='!", tg[pos].line, tg[pos].column);
        node->val = gen_valExprNode();
        if(peek()->content == ";") node->end = token();
        else throw epperr::Epperr("SyntaxError", "Expect ';'", tg[pos].line, tg[pos].column);
        return node;
    }
    else throw epperr::Epperr("SyntaxError", "It is not a proper Assign statement format", tg[pos].line, tg[pos].column);
}
east::DeleteStmtNode* east::astParser::gen_delStmtNode(){
    if(east::DeleteStmtNode::is_it(*this)){
        east::DeleteStmtNode* node = new east::DeleteStmtNode;
        node->mark = token();
        if(east::IdentifierNode::is_it(*this)) node->iden = gen_identifierNode();
        else throw epperr::Epperr("SyntaxError", "requires an identifier to delete", tg[pos].line, tg[pos].column);
        if(peek()->content == ";") node->end = token();
        else throw epperr::Epperr("SyntaxError", "Expect ';'", tg[pos].line, tg[pos].column);
        return node;
    }
    else throw epperr::Epperr("SyntaxError", "It is not a proper Delete statement format", tg[pos].line, tg[pos].column);
}
east::BlockStmtNode* east::astParser::gen_blockStmtNode(){
    if(east::BlockStmtNode::is_it(*this)){
        east::BlockStmtNode* node = new east::BlockStmtNode;
        node->left = token();
        if(east::StatNode::is_it(*this)) node->body = gen_statNode();
        if(peek()->content == "}") node->right = token();
        else throw epperr::Epperr("SyntaxError", "Expect '}'", tg[pos].line, tg[pos].column);
        return node;
    }
    else throw epperr::Epperr("SyntaxError", "It is not a proper Block statement format", tg[pos].line, tg[pos].column);
}
east::IfStmtNode* east::astParser::gen_ifStmtNode(){
    if(east::IfStmtNode::is_it(*this)){
        east::IfStmtNode* node = new east::IfStmtNode;
        node->mark = token();
        if(peek()->content == "(") node->left = token();
        else throw epperr::Epperr("SyntaxError", "Expect '('", tg[pos].line, tg[pos].column);
        if(east::ValExprNode::is_it(*this)) node->cond = gen_valExprNode();
        else throw epperr::Epperr("SyntaxError", "requires a boolean expression to supply to the if statement", tg[pos].line, tg[pos].column);
        if(peek()->content == ")") node->right = token();
        else throw epperr::Epperr("SyntaxError", "Expect ')'", tg[pos].line, tg[pos].column);
        if(east::BlockStmtNode::is_it(*this)) node->body = gen_blockStmtNode();
        else if(east::StmtNode::is_it(*this)) node->stc = gen_stmtNode();
        else throw epperr::Epperr("SyntaxError", "There is at least one statement under the if statement", tg[pos].line, tg[pos].column);
        return node;
    }
    else throw epperr::Epperr("SyntaxError", "It is not a proper If statement format", tg[pos].line, tg[pos].column);
}
east::ElseifStmtNode* east::astParser::gen_elifStmtNode(){
    if(east::ElseifStmtNode::is_it(*this)){
        east::ElseifStmtNode* node = new east::ElseifStmtNode;
        node->mark = token();
        if(peek()->content == "(") node->left = token();
        else throw epperr::Epperr("SyntaxError", "Expect '('", tg[pos].line, tg[pos].column);
        if(east::ValExprNode::is_it(*this)) node->cond = gen_valExprNode();
        else throw epperr::Epperr("SyntaxError", "requires a boolean expression to supply to the elif statement", tg[pos].line, tg[pos].column);
        if(peek()->content == ")") node->right = token();
        else throw epperr::Epperr("SyntaxError", "Expect ')'", tg[pos].line, tg[pos].column);
        if(east::BlockStmtNode::is_it(*this)) node->body = gen_blockStmtNode();
        else if(east::StmtNode::is_it(*this)) node->stc = gen_stmtNode();
        else throw epperr::Epperr("SyntaxError", "There is at least one statement under the elif statement", tg[pos].line, tg[pos].column);
        return node;
    }
    else throw epperr::Epperr("SyntaxError", "It is not a proper Else-If statement format", tg[pos].line, tg[pos].column);
}
east::ElseStmtNode* east::astParser::gen_elseStmtNode(){
    if(east::ElseStmtNode::is_it(*this)){
        east::ElseStmtNode* node = new east::ElseStmtNode;
        node->mark = token();
        if(east::BlockStmtNode::is_it(*this)) node->body = gen_blockStmtNode();
        else if(east::StmtNode::is_it(*this)) node->stc = gen_stmtNode();
        else throw epperr::Epperr("SyntaxError", "There is at least one statement under the else statement", tg[pos].line, tg[pos].column);
        return node;
    }
    else throw epperr::Epperr("SyntaxError", "It is not a proper Else statement format", tg[pos].line, tg[pos].column);
}
east::BreakStmtNode* east::astParser::gen_brkStmtNode(){
    if(east::BreakStmtNode::is_it(*this)){
        east::BreakStmtNode* node = new east::BreakStmtNode;
        node->mark = token();
        if(peek()->content == ";") node->end = token();
        else throw epperr::Epperr("SyntaxError", "Expect ';'", tg[pos].line, tg[pos].column);
        return node;
    }
    else throw epperr::Epperr("SyntaxError", "It is not a proper Break statement format", tg[pos].line, tg[pos].column);
}
east::ForEachStmtNode* east::astParser::gen_foreachStmtNode(){
    if(east::ForEachStmtNode::is_it(*this)){
        east::ForEachStmtNode* node = new east::ForEachStmtNode;
        node->mark = token();
        if(peek()->content == "(") node->left = token();
        else throw epperr::Epperr("SyntaxError", "Expect '('", tg[pos].line, tg[pos].column);
        if(peek()->content == "var") node->var_mark = token();
        else throw epperr::Epperr("SyntaxError", "Expect keyword: 'var'", tg[pos].line, tg[pos].column);
        if(peek()->type == "__IDENTIFIER__") node->iden = token();
        else throw epperr::Epperr("SyntaxError", "Expect an identifier", tg[pos].line, tg[pos].column);
        if(peek()->content == ":") node->mh = token();
        else throw epperr::Epperr("SyntaxError", "Expect ':'", tg[pos].line, tg[pos].column);
        if(peek()->type == "__IDENTIFIER__" || east::ListExprNode::is_it(*this)) node->ariden = gen_valExprNode();
        else throw epperr::Epperr("SyntaxError", "Expect an identifier", tg[pos].line, tg[pos].column);
        if(peek()->content == ")") node->right = token();
        else throw epperr::Epperr("SyntaxError", "Expect ')'", tg[pos].line, tg[pos].column);
        if(east::BlockStmtNode::is_it(*this)) node->body = gen_blockStmtNode();
        else if(east::StmtNode::is_it(*this)) node->stc = gen_stmtNode();
        else throw epperr::Epperr("SyntaxError", "There is at least one statement under the for_each statement", tg[pos].line, tg[pos].column);
        return node;
    }
    else throw epperr::Epperr("SyntaxError", "It is not a proper For_each statement format", tg[pos].line, tg[pos].column);
}
east::AreaStmtNode* east::astParser::gen_areaStmtNode(){
    if(east::AreaStmtNode::is_it(*this)){
        east::AreaStmtNode* node = new east::AreaStmtNode;
        node->mark = token();
        if(peek()->type == "__IDENTIFIER__") node->iden = token();
        else throw epperr::Epperr("SyntaxError", "Expect an identifier", tg[pos].line, tg[pos].column);
        if(east::BlockStmtNode::is_it(*this)) node->body = gen_blockStmtNode();
        else throw epperr::Epperr("SyntaxError", "There is block_stmt under the area statement", tg[pos].line, tg[pos].column);
        return node;
    }
    else throw epperr::Epperr("SyntaxError", "It is not a proper Area statement format", tg[pos].line, tg[pos].column);
}
east::WhileStmtNode* east::astParser::gen_whileStmtNode(){
    if(east::WhileStmtNode::is_it(*this)){
        east::WhileStmtNode* node = new east::WhileStmtNode;
        node->mark = token();
        if(peek()->content == "(") node->left = token();
        else throw epperr::Epperr("SyntaxError", "Expect '('", tg[pos].line, tg[pos].column);
        if(east::ValExprNode::is_it(*this)) node->cond = gen_valExprNode();
        else throw epperr::Epperr("SyntaxError", "requires a boolean expression to supply to the if statement", tg[pos].line, tg[pos].column);
        if(peek()->content == ")") node->right = token();
        else throw epperr::Epperr("SyntaxError", "Expect ')'", tg[pos].line, tg[pos].column);
        if(east::BlockStmtNode::is_it(*this)) node->body = gen_blockStmtNode();
        else if(east::StmtNode::is_it(*this)) node->stc = gen_stmtNode();
        else throw epperr::Epperr("SyntaxError", "There is at least one statement under the if statement", tg[pos].line, tg[pos].column);
        return node;
    }
    else throw epperr::Epperr("SyntaxError", "It is not a proper While statement format", tg[pos].line, tg[pos].column);
}
east::RepeatStmtNode* east::astParser::gen_reptStmtNode(){
    if(east::RepeatStmtNode::is_it(*this)){
        east::RepeatStmtNode* node = new east::RepeatStmtNode;
        node->mark = token();
        if(peek()->content == "(") node->left = token();
        else throw epperr::Epperr("SyntaxError", "Expect '('", tg[pos].line, tg[pos].column);
        if(east::ValExprNode::is_it(*this)) node->cond = gen_valExprNode();
        else throw epperr::Epperr("SyntaxError", "requires a boolean expression to supply to the if statement", tg[pos].line, tg[pos].column);
        if(peek()->content == ")") node->right = token();
        else throw epperr::Epperr("SyntaxError", "Expect ')'", tg[pos].line, tg[pos].column);
        if(east::BlockStmtNode::is_it(*this)) node->body = gen_blockStmtNode();
        else if(east::StmtNode::is_it(*this)) node->stc = gen_stmtNode();
        else throw epperr::Epperr("SyntaxError", "There is at least one statement under the if statement", tg[pos].line, tg[pos].column);
        return node;
    }
    else throw epperr::Epperr("SyntaxError", "It is not a proper Repeat statement format", tg[pos].line, tg[pos].column);
}

//

//identifier node
east::IdentifierNode::IdentifierNode(std::string type_) : type(type_){

}
std::string east::IdentifierNode::to_string(){
    if(this->type == "__PURE__"){
        std::string ret = "identifier_node(PURE): {[";
        ret += this->idens[0]->simply_format() + ", ";
        for(int i = 0; i < this->dots.size(); i++){
            ret += this->dots[i]->simply_format() + ", ";
            ret += this->idens[i + 1]->simply_format() + ", ";
        }
        ret += "]}";
        return ret;
    }
    else if(this->type == "__ARRE__"){
        return "identifier_node(ARRE): {" + this->idens[0]->content + ", index: " + this->arrindex->to_string() + "}";
    }
    return "__UNKNOWN__";
}
std::string east::IdentifierNode::getIdenType(){
    return this->type;
}
bool east::IdentifierNode::is_it(east::astParser ap){
    return ap.peek()->type == "__IDENTIFIER__" && (ap.peek(1)->content != "++" || ap.peek(1)->content != "--");
}
//

//assign expr node
std::string east::AssignExprNode::to_string(){
    return "assign_stmt: {[" + iden->to_string() + "]" + this->equ->simply_format() + ", " + this->val->to_string() + ", " + this->end->simply_format() + "}";
}
bool east::AssignExprNode::is_it(east::astParser ap){
    if(east::IdentifierNode::is_it(ap)){
        int temp = ap.pos;
        ap.gen_identifierNode();
        if(ap.peek()->content == "=") {ap.pos = temp; return true;}
        else {ap.pos = temp; return false;}
    }
    return false;
}
//

//whole expr node
std::string east::WholeExprNode::to_string(){
    if(this->assignexpr !=nullptr)
        return "whole_expr: {" + assignexpr->to_string() + "}";
    else if(this->valexpr != nullptr)
        return "whole_expr: {" + valexpr->to_string() + "}";
}
bool east::WholeExprNode::is_it(east::astParser ap){
    return east::AssignExprNode::is_it(ap) ||east::ValExprNode::is_it(ap);
}
//

//expr node
std::string east::ValExprNode::to_string(){
    if(this->addexpr != nullptr) return this->addexpr->to_string();
    else if(this->boolexpr != nullptr) return this->boolexpr->to_string();
    else if(this->listexpr != nullptr) return this->listexpr->to_string();
    else if(this->fdefexpr != nullptr) return this->fdefexpr->to_string();
    else throw epperr::Epperr("SyntaxError", "Unknown type of the expr!", 0, 0);
}
bool east::ValExprNode::is_it(east::astParser ap){
    return east::AddExprNode::is_it(ap) || east::BoolExprNode::is_it(ap) || east::ListExprNode::is_it(ap) || east::FuncDefineExprNode::is_it(ap);
}
//

//prim node
std::string east::PrimExprNode::to_string(){
    if(number != nullptr)
        return "prim_expr(NUMBER): {" + this->number->simply_format() + "}";
    else if(iden != nullptr)
        return "prim_expr(IDENTIFIER): {" + this->iden->to_string() + "}";
    else if(boolconst != nullptr)
        return "prim_expr(BOOL_CONST): {" + this->boolconst->simply_format() + "}";
    else if(ch != nullptr)
        return "prim_expr(CHAR): {" + this->ch->simply_format() + "}";
    else if(str != nullptr)
        return "prim_expr(STRING): {" + this->str->simply_format() + "}";
    else if(addexpr != nullptr)
        return "prim_expr(ADDEXPR): {" + this->addexpr->to_string() + "}";
    else if(boolexpr != nullptr)
        return "prim_expr(BOOLEXPR): {" + this->boolexpr->to_string() + "}";
    else if(tpof != nullptr)
        return "prim_expr(TPOF): {" + this->tpof->to_string() + "}";
    else if(input != nullptr)
        return "prim_expr(INPUT): {" + this->input->to_string() + "}";
    else if(glen != nullptr)
        return "prim_expr(LEN): {" + this->glen->to_string() + "}";
    else if(typeto != nullptr)
        return "prim_expr(TYPETO): {" + this->typeto->to_string() + "}";
    else if(fcall != nullptr)
        return "prim_expr(FUNC_CALL): {" + this->fcall->to_string() + "}";
    else if(siad != nullptr)
        return "prim_expr(SIAD): {" + this->siad->to_string() + "}";
    else if(poln != nullptr)
        return "prim_expr(PRINT/LN): {" + this->poln->to_string() + "}";
    else return "__NULL__";
}
bool east::PrimExprNode::is_it(east::astParser ap){
    return ap.peek()->type == "__IDENTIFIER__" || ap.peek()->type == "__NUMBER__" || ap.peek()->type == "__STRING__"||
           ap.peek()->type == "__CHAR__"|| ap.peek()->content == "(" || ap.peek()->content == "typeof" ||
           ap.peek()->content == "true"||ap.peek()->content == "false"|| ap.peek()->type == "__BIFIDEN__" || east::FuncCallExprNode::is_it(ap) ||
           east::SelfIaDExprNode::is_it(ap) || east::PrintoLnExprNode::is_it(ap);
}
//

//add op node
std::string east::AddOpNode::to_string(){
    return "add_op: {" + this->op->simply_format() + "}";
}
bool east::AddOpNode::is_it(east::astParser ap){
    return ap.peek()->content == "+" || ap.peek()->content == "-";
}
//

//mul op node
std::string east::MulOpNode::to_string(){
    return "mul_op: {" + this->op->simply_format() + "}";
}
bool east::MulOpNode::is_it(east::astParser ap){
    return ap.peek()->content == "*" || ap.peek()->content == "/"|| ap.peek()->content == "%";
}
//

//cmp op node
std::string east::CmpOpNode::to_string(){
    return "cmp_op: {" + this->op->simply_format() + "}";
}
bool east::CmpOpNode::is_it(east::astParser ap){
    return ap.peek()->content == "==" || ap.peek()->content == "!=" || ap.peek()->content == ">=" || ap.peek()->content == "<="
        || ap.peek()->content == ">" || ap.peek()->content == "<";
}
//

//bool op node
std::string east::BoolOpNode::to_string(){
    return "bool_op: {" + this->op->simply_format() + "}";
}
bool east::BoolOpNode::is_it(east::astParser ap){
    return ap.peek()->content == "&&" || ap.peek()->content == "||";
}
//

//cmp expr node
std::string east::CmpExprNode::to_string(){
    if(op != nullptr)
        return "cmp_expr: {" + this->expr->to_string() + ", symbol:" + this->op->to_string() + ", " + this->target->to_string() + "}";
    else
        return "cmp_expr: {" + this->expr->to_string()+ "}";
}
bool east::CmpExprNode::is_it(east::astParser ap){
    if(ap.peek()->content == "true" || ap.peek()->content == "false") return true;
    int temp = ap.pos;
    if(east::AddExprNode::is_it(ap)) {
        ap.gen_addExprNode();
        if(east::CmpOpNode::is_it(ap)) {ap.pos = temp; return true;}
        else return false;
    }
    else return false;
}
//

//bool expr node
std::string east::BoolExprNode::to_string(){
    std::string ret = "bool_expr: {[";
    ret += this->cmps[0]->to_string();
    for(int i = 0; i < this->ops.size(); i++){
        ret += this->ops[i]->to_string() + ", ";
        ret += this->cmps[i + 1]->to_string() + ", ";
    }
    ret += "]}";
    return ret;
}
bool east::BoolExprNode::is_it(east::astParser ap){
    return ap.peek()->content == "!" || east::CmpExprNode::is_it(ap);
}
//

//mul expr node
std::string east::MulExprNode::to_string(){
    std::string ret = "mul_expr: {[";
    ret += this->prims[0]->to_string();
    for(int i = 0; i < this->ops.size(); i++){
        ret += this->ops[i]->to_string() + ", ";
        ret += this->prims[i + 1]->to_string() + ", ";
    }
    ret += "]}";
    return ret;
}
bool east::MulExprNode::is_it(east::astParser ap){
    return east::PrimExprNode::is_it(ap);
}
//

//selfiad expr node
std::string east::SelfIaDExprNode::to_string(){
    return "selfiad_expr: {" + this->op->simply_format() + ", " + this->iden->to_string() + "}";
}
bool east::SelfIaDExprNode::is_it(east::astParser ap){
    if(ap.peek()->content == "++" || ap.peek()->content == "--") return true;
    else if(east::IdentifierNode::is_it(ap)){
        int temp = ap.pos;
        ap.gen_identifierNode();
        if(ap.peek()->content == "++" || ap.peek()->content == "--") {ap.pos = temp; return true;}
        else {ap.pos = temp; return false;}
        return false;
    }
    return false;
}

//add expr node
std::string east::AddExprNode::to_string(){
    std::string ret = "add_expr: {[";
    ret += this->muls[0]->to_string();
    for(int i = 0; i < this->ops.size(); i++){
        ret += this->ops[i]->to_string() + ", ";
        ret += this->muls[i + 1]->to_string() + ", ";
    }
    ret += "]}";
    return ret;
}
bool east::AddExprNode::is_it(east::astParser ap){
    return east::PrimExprNode::is_it(ap);
}
//

//list expr node
std::string east::ListExprNode::to_string(){
    std::string ret = "list_expr: {" + this->left->simply_format() + ", [";
    ret += arrayelts[0]->to_string();
    for(int i = 0; i<arrayelts.size(); i++){
        ret += arrayelts[i]->to_string();
    }
    ret += "], ";
    ret += this->right->simply_format() + "}";
    return ret;
}
bool east::ListExprNode::is_it(astParser ap){
    return ap.peek()->content == "[";
}
//

//typeof expr node
std::string east::TypeOfExprNode::to_string(){
    return "typeof_expr: {" + mark->simply_format() + ", " + expr->to_string() + "}";
}
bool east::TypeOfExprNode::is_it(east::astParser ap){
    return ap.peek()->content == "typeof";
}
//

//typeto expr node
std::string east::TypeToExprNode::to_string(){
    return "typeto_expr: {" + mark->simply_format() + ", " + expr->to_string() + "}";
}
bool east::TypeToExprNode::is_it(east::astParser ap){
    return ap.peek()->content == "str" || ap.peek()->content == "int" || ap.peek()->content == "deci"  || ap.peek()->content == "bool";
}
//

//printoln expr node
std::string east::PrintoLnExprNode::to_string(){
    return "print/ln_expr: {" + mark->simply_format() + ", " + expr->to_string() + "}";
}
bool east::PrintoLnExprNode::is_it(east::astParser ap){
    return ap.peek()->content == "print" || ap.peek()->content == "println";
}
//

//func define expr node
std::string east::FuncDefineExprNode::to_string(){
    std::string ret = "func_define_expr: {" + mark->simply_format() + ",[";
    if(paras.empty()){
        ret += "], " + body->to_string() + "}";
    }
    else {
        ret += paras[0]->to_string();
        for(int i = 0; i<paras.size(); i++){
            ret += paras[i]->to_string();
        }
        ret += "], " + body->to_string() + "}";
    }
    return ret;
}
bool east::FuncDefineExprNode::is_it(east::astParser ap){
    return ap.peek()->content == "func";
}
//

//func call expr node
std::string east::FuncCallExprNode::to_string(){
    std::string ret = "func_call_expr: {" + func_name->to_string() + ",[";
    if(act_paras.empty()){
        ret += "]}";
    }
    else{
        ret += act_paras[0]->to_string();
        for(int i = 0; i<act_paras.size(); i++){
            ret += act_paras[i]->to_string();
        }
        ret += "]}";
    }
    return ret;
}
bool east::FuncCallExprNode::is_it(east::astParser ap){
    if(east::IdentifierNode::is_it(ap)){
        int temp = ap.pos;
        ap.gen_identifierNode();
        if(ap.peek()->content == "(") {ap.pos = temp; return true;}
        else {ap.pos = temp; return false;}
    }
    return false;
}
//

//len expr node
std::string east::LenExprNode::to_string(){
    return "len_expr: {" + mark->simply_format() + ", " + expr->to_string() + "}";
}
bool east::LenExprNode::is_it(east::astParser ap){
    return ap.peek()->content == "len";
}
//

//input expr node
std::string east::InputExprNode::to_string(){
    if(expr != nullptr)
        return "input_expr: {" + mark->simply_format() + ", " + expr->to_string() + "}";
    else
        return "input_expr: {" + mark->simply_format()  + "}";
}
bool east::InputExprNode::is_it(east::astParser ap){
    return ap.peek()->content == "input";
}
//

//stmt node
std::string east::StmtNode::to_string(){
    if(outstmt != nullptr) return "stmt_node: {" + this->outstmt->to_string() + "}";
    else if(vorcstmt != nullptr) return "stmt_node: {" + this->vorcstmt->to_string() + "}";
    else if(assignstmt != nullptr) return "stmt_node: {" + this->assignstmt->to_string() + "}";
    else if(deletestmt != nullptr) return "stmt_node: {" + this->deletestmt->to_string() + "}";
    else if(ifstmt != nullptr) return "stmt_node: {" + this->ifstmt->to_string() + "}";
    else if(elifstmt != nullptr) return "stmt_node: {" + this->elifstmt->to_string() + "}";
    else if(elsestmt != nullptr) return "stmt_node: {" + this->elsestmt->to_string() + "}";
    else if(blockstmt != nullptr) return "stmt_node: {" + this->blockstmt->to_string() + "}";
    else if(brkstmt != nullptr) return "stmt_node: {" + this->brkstmt->to_string() + "}";
    else if(whilestmt != nullptr) return "stmt_node: {" + this->whilestmt->to_string() + "}";
    else if(reptstmt != nullptr) return "stmt_node: {" + this->reptstmt->to_string() + "}";
    else if(foreachstmt != nullptr) return "stmt_node: {" + this->foreachstmt->to_string() + "}";
    else if(areastmt != nullptr) return "stmt_node: {" + this->areastmt->to_string() + "}";
    else return "__NULL__";
}
bool east::StmtNode::is_it(east::astParser ap){
    return east::OutStmtNode::is_it(ap) || east::VorcStmtNode::is_it(ap) || east::AssignStmtNode::is_it(ap) || east::DeleteStmtNode::is_it(ap)
            || east::BlockStmtNode::is_it(ap) || east::IfStmtNode::is_it(ap) || east::RepeatStmtNode::is_it(ap) || east::WhileStmtNode::is_it(ap)
            || east::BreakStmtNode::is_it(ap) || east::ElseStmtNode::is_it(ap) || east::ElseifStmtNode::is_it(ap) || east::ForEachStmtNode::is_it(ap)
            || east::AreaStmtNode::is_it(ap);
}
//

//stat node
std::string east::StatNode::to_string(){
    std::string ret = "stat_node: {[";
    for(auto stmt : stmts){
        ret += stmt->to_string() + ", ";
    }
    ret += "]}";
    return ret;
}
bool east::StatNode::is_it(east::astParser ap){
    return east::StmtNode::is_it(ap);
}
//

//out stmt node
std::string east::OutStmtNode::to_string(){
    return "out_stmt: { " + this->mark->simply_format() + ", " + this->content->to_string() + ", " + this->end->simply_format();
}
bool east::OutStmtNode::is_it(east::astParser ap){
    return ap.peek()->content == "out";
}
//

//vorc stmt node
std::string east::VorcStmtNode::to_string(){
    return "vorc_stmt: {" + this->mark->simply_format() + "," + this->iden->simply_format() + (this->type!=nullptr?", " + this->type->simply_format():"") + ", " + this->equ->simply_format() + ", " + this->value->to_string() + ", "  + this->end->simply_format() + "}";
}
bool east::VorcStmtNode::is_it(east::astParser ap){
    return ap.peek()->content == "var" || ap.peek()->content == "const";
}
//

//assign stmt node
std::string east::AssignStmtNode::to_string(){
    return "assign_stmt: {[" + iden->to_string() + "]" + this->equ->simply_format() + ", " + this->val->to_string() + ", " + this->end->simply_format() + "}";
}
bool east::AssignStmtNode::is_it(east::astParser ap){
    if(east::IdentifierNode::is_it(ap)){
        int temp = ap.pos;
        ap.gen_identifierNode();
        if(ap.peek()->content == "=") {ap.pos = temp; return true;}
        else {ap.pos = temp; return false;}
    }
    return false;
}
//

//block stmt node
std::string east::BlockStmtNode::to_string(){
    std::string ret = "block_stmt: {" + this->left->simply_format() + ", body:[";
    ret += this->body->stmts[0]->to_string();
    for(int i = 0; i<this->body->stmts.size(); i++){
        ret += this->body->stmts[i]->to_string();
    }
    ret += "]}";
    return ret;
}
bool east::BlockStmtNode::is_it(astParser ap){
    return ap.peek()->content == "{";
}
//

//delete stmt node
std::string east::DeleteStmtNode::to_string(){
    return "delete_stmt: {" + this->mark->simply_format() + ", " + this->iden->to_string() + ", " + end->simply_format() + "}";
}
bool east::DeleteStmtNode::is_it(east::astParser ap){
    return ap.peek()->content == "delete";
}
//

//if stmt node
std::string east::IfStmtNode::to_string(){
    if(body != nullptr)
        return "if_stmt: {" + this->mark->simply_format() + ", " + this->cond->to_string() + ", " + this->body->to_string() + "}";
    else
        return "if_stmt: {" + this->mark->simply_format() + ", " + this->cond->to_string() + ", " + this->stc->to_string() + "}";
}
bool east::IfStmtNode::is_it(east::astParser ap){
    return ap.peek()->content == "if";
}
//

//if stmt node
std::string east::ElseifStmtNode::to_string(){
    if(body != nullptr)
        return "elif_stmt: {" + this->mark->simply_format() + ", " + this->cond->to_string() + ", " + this->body->to_string() + "}";
    else
        return "elif_stmt: {" + this->mark->simply_format() + ", " + this->cond->to_string() + ", " + this->stc->to_string() + "}";
}
bool east::ElseifStmtNode::is_it(east::astParser ap){
    return ap.peek()->content == "elif";
}
//

//else stmt node
std::string east::ElseStmtNode::to_string(){
    if(body != nullptr)
        return "if_stmt: {" + this->mark->simply_format() + ", " + this->body->to_string() + "}";
    else
        return "if_stmt: {" + this->mark->simply_format() + ", " + this->stc->to_string() + "}";
}
bool east::ElseStmtNode::is_it(east::astParser ap){
    return ap.peek()->content == "else";
}
//

//while stmt node
std::string east::WhileStmtNode::to_string(){
    return "while_stmt: {" + mark->simply_format() + ", " = cond->to_string() + ", " + body->to_string() + "}";
}
bool east::WhileStmtNode::is_it(east::astParser ap){
    return ap.peek()->content == "while";
}
//

//repeat stmt node
std::string east::RepeatStmtNode::to_string(){
    return "repeat_stmt: {" + mark->simply_format() + ", " = cond->to_string() + ", " + body->to_string() + "}";
}
bool east::RepeatStmtNode::is_it(east::astParser ap){
    return ap.peek()->content == "repeat";
}
//

//break stmt node
std::string east::BreakStmtNode::to_string(){
    return "break_stmt: {" + mark->simply_format() + "}";
}
bool east::BreakStmtNode::is_it(east::astParser ap){
    return ap.peek()->content == "break";
}
//

//for_each stmt node
std::string east::ForEachStmtNode::to_string(){
    return "for_each_stmt_node: {" + mark->simply_format() + ", " + iden->simply_format() + ", " + ariden->to_string() + ", " + (this->stc!=nullptr?stc->to_string():body->to_string()) + "}";
}
bool east::ForEachStmtNode::is_it(east::astParser ap){
    return ap.peek()->content == "for_each";
}
//

//area stmt node
std::string east::AreaStmtNode::to_string(){
    return "area_stmt_node: {" + mark->simply_format() + ", " + iden->simply_format() + ", " + body->to_string() + "}";
}
bool east::AreaStmtNode::is_it(east::astParser ap){
    return ap.peek()->content == "area";
}
//
#include "../svf-onnx/CheckModels.h"
#include "../svf-onnx/Solver.h"
#include "Graphs/NNGraph.h"
#include "SVFIR/SVFIR.h"
#include "Util/SVFUtil.h"
#include "iomanip"

using namespace SVF;

/// ReLu

NeuronNode::NodeK ReLuNeuronNode::get_type() const{
    return ReLuNode;
}


/// filter
unsigned int FilterSubNode::get_depth() const{
    return value.size();
}

unsigned int FilterSubNode::get_height() const{
    if (value.size() > 0) {
        /// The first channel
        return value[0].rows();
    }
    else {
        return 0;
    }
}

unsigned int FilterSubNode::get_width() const{
    if (value.size() > 0) {
        return value[0].cols();
    }
    else {
        return 0;
    }
}

double FilterSubNode::dot_product(const FilterSubNode& val_f) const{
    double sum = 0.0;
    /// Test the size of windows
    if (val_f.get_depth() != get_depth()) {
        throw std::runtime_error("Dimension-d mismatch in Filter.dot_product");
    }
    if (val_f.get_height() != get_height()) {
        throw std::runtime_error("Dimension-h mismatch in Filter.dot_product");
    }
    if (val_f.get_width() != get_width()) {
        throw std::runtime_error("Dimension-w mismatch in Filter.dot_product");
    }

    /// dot
    for (unsigned i = 0; i < get_depth(); i++) {
        for (unsigned j = 0; j < get_height(); j++) {
            for (unsigned k = 0; k < get_width(); k++) {
                sum += val_f.value[i](j, k) * value[i](j, k);
            }
        }
    }
    return sum;
}

/// BasicOPNeuronNode:including Sub, Mul, Div, and Add.
NeuronNode::NodeK BasicOPNeuronNode::get_type() const{
    if(oper == "Sub"){
        return Sub;
    }else if(oper == "Add"){
        return Add;
    }else if(oper == "Mul"){
        return Mul;
    }else if(oper == "Div"){
        return Div;
    }
    return BasicOPNode;
}



/// Maxpooling

NeuronNode::NodeK MaxPoolNeuronNode::get_type() const{
    return MaxPoolNode;
}

unsigned int MaxPoolNeuronNode::get_window_width() const{
    return window_height;
}

unsigned int MaxPoolNeuronNode::get_window_height() const{
    return window_width;
}

unsigned int MaxPoolNeuronNode::get_stride_width() const{
    return stride_width;
}

unsigned int MaxPoolNeuronNode::get_stride_height() const{
    return stride_height;
}

unsigned int MaxPoolNeuronNode::get_pad_width() const{
    return pad_width;
}

unsigned int MaxPoolNeuronNode::get_pad_height() const{
    return pad_height;
}


/// FullyCon

NeuronNode::NodeK FullyConNeuronNode::get_type() const{
    return FullyConNode;
}

Eigen::MatrixXd FullyConNeuronNode::get_weight() const{
    return weight;
}

Eigen::VectorXd FullyConNeuronNode::get_bias() const{
    return bias;
}


/// Conv
NeuronNode::NodeK ConvNeuronNode::get_type() const{
    return ConvNode;
}

/// Constant
NeuronNode::NodeK ConstantNeuronNode::get_type() const{
    return ConstantNode;
}


/// ques
void NeuronNode::dump() const{
    SVFUtil::outs() << this->toString() << "\n";
}

const std::string ReLuNeuronNode::toString() const{
    std::string str;
    std::stringstream rawstr(str);
    rawstr <<ReLuNeuronNode::get_type() << "NNNode" << getId();
    return rawstr.str();
}

const std::string BasicOPNeuronNode::toString() const{
    std::string str;
    std::stringstream rawstr(str);
    rawstr << BasicOPNeuronNode::get_type() <<BasicOPNeuronNode::get_type() << "NNNode" << getId();
    return rawstr.str();
}

const std::string MaxPoolNeuronNode::toString() const{
    std::string str;
    std::stringstream rawstr(str);
    rawstr << MaxPoolNeuronNode::get_type() << "NNNode" << getId();
    return rawstr.str();
}

const std::string FullyConNeuronNode::toString() const{
    std::string str;
    std::stringstream rawstr(str);
    rawstr << FullyConNeuronNode::get_type() << "NNNode" << getId();
    return rawstr.str();
}

const std::string ConvNeuronNode::toString() const{
    std::string str;
    std::stringstream rawstr(str);
    rawstr << ConvNeuronNode::get_type() << "NNNode" << getId();
    return rawstr.str();
}

const std::string ConstantNeuronNode::toString() const{
    std::string str;
    std::stringstream rawstr(str);
    rawstr << ConstantNeuronNode::get_type() << "NNNode" << getId();
    return rawstr.str();
}

/// Edge
const std::string Direct2NeuronEdge::toString() const{
    std::string str;
    std::stringstream rawstr(str);
    rawstr << "NNEdge: [NNNode" << getDstID() << " <-- NNNode" << getSrcID() << "]\t";
    return rawstr.str();
}

const std::string NeuronEdge::toString() const{
    std::string str;
    std::stringstream rawstr(str);
    rawstr << "NNEdge: [NNNode" << getDstID() << " <-- NNNode" << getSrcID() << "]\t";
    return rawstr.str();
}

NeuronNet::~NeuronNet()
{}

NeuronEdge* NeuronNet::hasNeuronEdge(NeuronNode* src, NeuronNode* dst, NeuronEdge::NeuronEdgeK kind)
{
    NeuronEdge edge(src, dst, kind);
    NeuronEdge* outEdge = src->hasOutgoingEdge(&edge);
    NeuronEdge* inEdge = dst->hasIncomingEdge(&edge);
    if (outEdge && inEdge)
    {
        assert(outEdge == inEdge && "edges not match");
        return outEdge;
    }
    else
        return nullptr;
}

NeuronEdge* NeuronNet::getNeuronEdge(const NeuronNode* src, const NeuronNode* dst, NeuronEdge::NeuronEdgeK kind){
    NeuronEdge* edge = nullptr;
    u32_t counter = 0;
    for(NeuronEdge::NeuronGraphEdgeSetTy::iterator iter = src->OutEdgeBegin();
         iter != src->OutEdgeEnd(); ++iter){
        if ((*iter)->getDstID() == dst->getId() && (*iter)->getEdgeKind() == kind)
        {
            counter++;
            edge = (*iter);
        }
    }
    assert(counter <= 1 && "there's more than one edge between two Neuron nodes");
    return edge;
}

const std::string NeuronNode::toString() const{
    std::string str;
    std::stringstream rawstr(str);
    rawstr << "NNNode" << getId();
    return rawstr.str();
}

void NeuronNet::dump(const std::string& file, bool simple)
{
    GraphPrinter::WriteGraphToFile(SVFUtil::outs(), file, this, simple);
}

void NeuronNet::view()
{
    ViewGraph(this, "SVF NeuronNet Graph");
}


namespace SVF
{
template <> struct DOTGraphTraits<NeuronNet*> : public DOTGraphTraits<SVFIR*>
{
    typedef NeuronNode NodeType;
    DOTGraphTraits(bool isSimple = false) : DOTGraphTraits<SVFIR*>(isSimple) {}

    /// Get the Graph's name
    static std::string getGraphName(NeuronNet*)
    {
        return "Neuronnet Graph";
    }

    static std::string getSimpleNodeLabel(NodeType* node, NeuronNet*)
    {
        return node->toString();
    }

    std::string getNodeLabel(NodeType* node, NeuronNet* graph)
    {
        return getSimpleNodeLabel(node, graph);
    }

    static std::string getNodeAttributes(NodeType* node, NeuronNet*)
    {
        std::string str;
        std::stringstream rawstr(str);

        if (SVFUtil::isa<ReLuNeuronNode>(node))
        {
            rawstr << "color=black";
        }
        else if (SVFUtil::isa<BasicOPNeuronNode>(node))
        {
            rawstr << "color=yellow";
        }
        else if (SVFUtil::isa<FullyConNeuronNode>(node))
        {
            rawstr << "color=green";
        }
        else if (SVFUtil::isa<ConvNeuronNode>(node))
        {
            rawstr << "color=red";
        }
        else if (SVFUtil::isa<MaxPoolNeuronNode>(node))
        {
            rawstr << "color=blue";
        }
        else if (SVFUtil::isa<ConstantNeuronNode>(node))
        {
            rawstr << "color=purple";
        }
        else
            assert(false && "no such kind of node!!");

        rawstr << "";

        return rawstr.str();
    }

    template <class EdgeIter>
    static std::string getEdgeAttributes(NodeType*, EdgeIter EI, NeuronNet*)
    {
        NeuronEdge* edge = *(EI.getCurrent());
        assert(edge && "No edge found!!");
        if (SVFUtil::isa<Direct2NeuronEdge>(edge))
            return "style=solid,color=red";
        else
            return "style=solid";
        return "";
    }

    template <class EdgeIter>
    static std::string getEdgeSourceLabel(NodeType*, EdgeIter EI)
    {
        NeuronEdge* edge = *(EI.getCurrent());
        assert(edge && "No edge found!!");

        std::string str;
        std::stringstream rawstr(str);

        rawstr << "";
        return rawstr.str();
    };

};
}


/// BY NODE
void GraphTraversal::printPath(std::vector<const NeuronNode *> &path){
    std::string output = "START: ";
    for (size_t i = 0; i < path.size(); ++i) {
        output += std::to_string(path[i]->getId());
        if (i < path.size() - 1) {
            output += "->";
        }
    }
    paths.insert(output);
};

NeuronNodeVariant GraphTraversal::convertToVariant(NeuronNode* node) {
    /// Check the specific type of node and construct NeuronNodeVariant accordingly
    if (auto* constantNode = SVFUtil::dyn_cast<ConstantNeuronNode>(node)) {
        return constantNode;
    } else if (auto* basicOPNode = SVFUtil::dyn_cast<BasicOPNeuronNode>(node)) {
        return basicOPNode;
    } else if (auto* fullyConNode = SVFUtil::dyn_cast<FullyConNeuronNode>(node)) {
        return fullyConNode;
    } else if (auto* convNode = SVFUtil::dyn_cast<ConvNeuronNode>(node)) {
        return convNode;
    } else if (auto* reLuNode = SVFUtil::dyn_cast<ReLuNeuronNode>(node)) {
        return reLuNode;
    } else if (auto* maxPoolNode = SVFUtil::dyn_cast<MaxPoolNeuronNode>(node)) {
        return maxPoolNode;
    }
    ///  If the node does not match any known type, it can return std:: monostate or throw an exception
    return std::monostate{};
}


bool GraphTraversal::checkNodeInVariant(const NeuronNode* current, const NeuronNodeVariant& dst) {
    return std::visit([current](auto&& arg) -> bool {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (!std::is_same_v<T, std::monostate>) {
            return current == static_cast<const NeuronNode*>(arg);
        } else {
            return false;
        }
    }, dst);
}

NeuronNode* GraphTraversal::getNeuronNodePtrFromVariant(const NeuronNodeVariant& variant) {
    return std::visit([](auto&& arg) -> NeuronNode* {
        using T = std::decay_t<decltype(arg)>;
        if constexpr (std::is_same_v<T, std::monostate>) {
            /// Corresponding to the situation where variant does not hold any NeuronNode pointers
            return nullptr;
        } else {
            /// Return NeuronNode' pointer
            return arg;
        }
    }, variant);
}


///  3.13
/// matrix into interval
void GraphTraversal::DFS(std::set<const NeuronNode *> &visited, std::vector<const NeuronNode *> &path, const NeuronNodeVariant *src, const NeuronNodeVariant *dst, std::vector<Eigen::MatrixXd> in_x) {
    std::stack<std::pair<NeuronNode*, std::vector<Eigen::MatrixXd>>> stack;

    /// Ensure that src is obtained by dereferencing NeuronNode
    stack.emplace(getNeuronNodePtrFromVariant(*src), in_x);

    SolverEvaluate solver(in_x);
    int i = 0;
    std::vector<Eigen::MatrixXd> IRRes;

    while (!stack.empty()) {
        std::cout<<" Node: "<<i<<std::endl;
        i++;
        auto currentPair = stack.top();
        stack.pop();

        const NeuronNode* current = currentPair.first;
        IRRes = currentPair.second;

        std::cout<<&current<<std::endl<<" STACK SIZE: "<<stack.size()<<std::endl;
        std::cout<<" The size of Matrix: "<<IRRes.size()<<std::endl;

        if (!visited.insert(current).second) {
            std::cout<<"CONTINUE: This Node "<<&current<<" has already been visited!"<<std::endl;
            continue;
        }

        path.push_back(current);

        if (checkNodeInVariant(current, *dst)) {
            printPath(path);
        }


        for (const auto& edge : current->getOutEdges()) {
            auto *neighbor = edge->getDstNode();

            NeuronNodeVariant variantNeighbor = convertToVariant(neighbor);
            std::cout<<"SrcNode: "<<current<<" ->  DSTNode: "<<neighbor<<" -> ConvertIns: "<<&variantNeighbor<<std::endl;

            std::cout<<"NodeTYpe:  "<<neighbor->get_type()<<std::endl;

            if (visited.count(neighbor) == 0) {
                /// Process IRRes based on the type of neighbor
                std::vector<Eigen::MatrixXd> newIRRes; /// Copy the current IRRes to avoid modifying the original data
                if (neighbor->get_type() == 0) {
                    solver.setIRMatrix(IRRes);
                    newIRRes = solver.ReLuNeuronNodeevaluate();
                    std::cout<<"FINISH RELU"<<std::endl;
                } else if (neighbor->get_type() == 1 || neighbor->get_type() == 6 || neighbor->get_type() == 7 || neighbor->get_type() == 8 || neighbor->get_type() == 9) {
                    const BasicOPNeuronNode *node = SVFUtil::dyn_cast<BasicOPNeuronNode>(neighbor);
                    solver.setIRMatrix(IRRes);
                    newIRRes = solver.BasicOPNeuronNodeevaluate(node);
                    std::cout<<"FINISH BAIC"<<std::endl;
                } else if (neighbor->get_type() == 2) {
                    const MaxPoolNeuronNode *node = SVFUtil::dyn_cast<MaxPoolNeuronNode>(neighbor);
                    solver.setIRMatrix(IRRes);
                    newIRRes = solver.MaxPoolNeuronNodeevaluate(node);
                    std::cout<<"FINISH MAXPOOLING"<<std::endl;
                } else if (neighbor->get_type() == 3) {
                    const ConvNeuronNode *node = static_cast<ConvNeuronNode *>(neighbor);
                    solver.setIRMatrix(IRRes);
                    newIRRes = solver.ConvNeuronNodeevaluate(node);
                    std::cout<<"FINISH Conv"<<std::endl;
                } else if (neighbor->get_type() == 4) {
                    const FullyConNeuronNode *node = SVFUtil::dyn_cast<FullyConNeuronNode>(neighbor);
                    solver.setIRMatrix(IRRes);
                    std::cout<<"*******************"<<std::endl;
                    newIRRes = solver.FullyConNeuronNodeevaluate(node);
                    std::cout<<"FINISH FullyConnected"<<std::endl;
                } else if (neighbor->get_type() == 5) {
                    solver.setIRMatrix(IRRes);
                    newIRRes = solver.ConstantNeuronNodeevaluate();
                    std::cout<<"FINISH Constant"<<std::endl;
                }
                /// Push neighbor and new IRRes into the stack
                IRRes = newIRRes;
                stack.emplace(neighbor, newIRRes);
                std::cout<<"FINISH PUSHING STACK! "<<stack.size()<<std::endl<<std::endl;
            }
        }

        /// print the IRRes Matrix
        std::cout << "IRRes content after the loop iteration:" << i << std::endl;
        std::cout.precision(20);
        std::cout << std::fixed;
        for (size_t j = 0; j < IRRes.size(); ++j) {
            std::cout << "Matrix " << j << ":\n";
            std::cout << "Rows: " << IRRes[j].rows() << ", Columns: " << IRRes[j].cols() << "\n";
            std::cout << IRRes[j] << "\n\n";
        }
        visited.erase(current);
        path.pop_back(); /// Remove current node
    }
}

///  3.13
/// matrix into interval
void GraphTraversal::IntervalDFS(std::set<const NeuronNode *> &visited, std::vector<const NeuronNode *> &path, const NeuronNodeVariant *src, const NeuronNodeVariant *dst, std::vector<Eigen::MatrixXd> in_x) {
    std::stack<std::pair<NeuronNode*, IntervalMatrices *>> stack;
    IntervalSolver solver(in_x);

    /// Ensure that src is obtained by dereferencing NeuronNode
    stack.emplace(getNeuronNodePtrFromVariant(*src), &solver.interval_data_matrix);

    int i = 0;
    IntervalMatrices IRRes;
    IntervalMatrices newIRRes; /// Copy the current IRRes to avoid modifying the original data

    while (!stack.empty()) {
        std::cout<<" Node: "<<i<<std::endl;
        i++;
        auto currentPair = stack.top();
        stack.pop();

        const NeuronNode* current = currentPair.first;

        IRRes = *currentPair.second;
        std::cout<<"*******IRRes:"<<IRRes.size()<<", "<<IRRes[0].rows()<<", "<<IRRes[0].cols()<<std::endl;

        std::cout<<&current<<std::endl<<" STACK SIZE: "<<stack.size()<<std::endl;
        std::cout<<" The size of Matrix: "<<IRRes.size()<<std::endl;

        if (!visited.insert(current).second) {
            std::cout<<"CONTINUE: This Node "<<&current<<" has already been visited!"<<std::endl;
            continue;
        }

        path.push_back(current);

        if (checkNodeInVariant(current, *dst)) {
            printPath(path);
        }

        for (const auto& edge : current->getOutEdges()) {
            auto *neighbor = edge->getDstNode();

            NeuronNodeVariant variantNeighbor = convertToVariant(neighbor);
            std::cout<<"SrcNode: "<<current<<" ->  DSTNode: "<<neighbor<<" -> ConvertIns: "<<&variantNeighbor<<std::endl;

            std::cout<<"NodeTYpe:  "<<neighbor->get_type()<<std::endl;

            if (visited.count(neighbor) == 0) {
                /// Process IRRes based on the type of neighbor
                if (neighbor->get_type() == 0) {
                    solver.setIRMatrix(IRRes);
                    newIRRes = solver.ReLuNeuronNodeevaluate();
                    std::cout<<"FINISH RELU, The Result size:  ("<<newIRRes.size()<<", "<<newIRRes[0].rows()<<", "<<newIRRes[0].cols()<<")"<<std::endl;
                } else if (neighbor->get_type() == 1 || neighbor->get_type() == 6 || neighbor->get_type() == 7 || neighbor->get_type() == 8 || neighbor->get_type() == 9) {
                    const BasicOPNeuronNode *node = SVFUtil::dyn_cast<BasicOPNeuronNode>(neighbor);
                    solver.setIRMatrix(IRRes);
                    newIRRes = solver.BasicOPNeuronNodeevaluate(node);
                    std::cout<<"FINISH BAIC, The Result size: ("<<newIRRes.size()<<", "<<newIRRes[0].rows()<<", "<<newIRRes[0].cols()<<")"<<std::endl;
                } else if (neighbor->get_type() == 2) {
                    const MaxPoolNeuronNode *node = SVFUtil::dyn_cast<MaxPoolNeuronNode>(neighbor);
                    solver.setIRMatrix(IRRes);
                    newIRRes = solver.MaxPoolNeuronNodeevaluate(node);
                    std::cout<<"FINISH MAXPOOLING, The Result size: ("<<newIRRes.size()<<", "<<newIRRes[0].rows()<<", "<<newIRRes[0].cols()<<")"<<std::endl;
                } else if (neighbor->get_type() == 3) {
                    const ConvNeuronNode *node = static_cast<ConvNeuronNode *>(neighbor);
                    solver.setIRMatrix(IRRes);
                    newIRRes = solver.ConvNeuronNodeevaluate(node);
                    std::cout<<"FINISH Conv, The Result size: ("<<newIRRes.size()<<", "<<newIRRes[0].rows()<<", "<<newIRRes[0].cols()<<")"<<std::endl;
                } else if (neighbor->get_type() == 4) {
                    const FullyConNeuronNode *node = SVFUtil::dyn_cast<FullyConNeuronNode>(neighbor);
                    solver.setIRMatrix(IRRes);
                    std::cout<<"*******************"<<std::endl;
                    newIRRes = solver.FullyConNeuronNodeevaluate(node);
                    std::cout<<"FINISH FullyConnected, The Result size: ("<<newIRRes.size()<<", "<<newIRRes[0].rows()<<", "<<newIRRes[0].cols()<<")"<<std::endl;
                } else if (neighbor->get_type() == 5) {
                    solver.setIRMatrix(IRRes);
                    newIRRes = solver.ConstantNeuronNodeevaluate();
                    std::cout<<"FINISH Constant, The Result size: ("<<newIRRes.size()<<", "<<newIRRes[0].rows()<<", "<<newIRRes[0].cols()<<")"<<std::endl;
                }
                /// Push neighbor and new IRRes into the stack
                IRRes = newIRRes;
                std::cout<<"Ready to push: "<<newIRRes.size()<<", "<<newIRRes[0].rows()<<", "<<newIRRes[0].cols()<<")"<<std::endl;
                stack.emplace(neighbor, &newIRRes);
                std::cout<<"FINISH PUSHING STACK! "<<stack.size()<< ",    Number of Intervalmatrix: " << newIRRes.size() <<std::endl<<std::endl;
            }
        }

        /// print the IRRes Matrix
        std::cout << "IRRes content after the loop iteration:" << i << ",    Number of Intervalmatrix: " << IRRes.size() <<std::endl;
        std::cout.precision(20);
        std::cout << std::fixed;
//        for(u32_t ii = 0; ii<IRRes.size(); ii++){
        for (const auto& intervalMat : IRRes) {
            std::cout << "IntervalMatrix :\n";
            std::cout << "Rows: " << intervalMat.rows() << ", Columns: " << intervalMat.cols() << "\n";
            for (int k = 0; k < intervalMat.rows(); ++k) {
                for (int j = 0; j < intervalMat.cols(); ++j) {
                    std::cout<< "[ "<< intervalMat(k, j).lb().getRealNumeral()<<", "<< intervalMat(k, j).ub().getRealNumeral() <<" ]"<< "\t";
                }
                std::cout << std::endl;
            }
            std::cout<<"****************"<<std::endl;
        }

        visited.erase(current);
        path.pop_back(); /// Remove current node
    }
}
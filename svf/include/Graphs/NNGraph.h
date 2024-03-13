#ifndef SVF_NNGRAPH_H
#define SVF_NNGRAPH_H

#include "NNEdge.h"
#include "NNNode.h"



/// using SVF namespace graph strcture
namespace SVF {

typedef GenericGraph<NeuronNode, NeuronEdge> GenericNeuronNetTy;
class NeuronNet : public GenericNeuronNetTy {
public:
    /// id to type
    typedef OrderedMap<NodeID, NeuronNode *> NeuronNetNodeID2NodeMapTy;
    typedef NeuronEdge::NeuronGraphEdgeSetTy NeuronGraphEdgeSetTy;
    typedef NeuronNetNodeID2NodeMapTy::iterator iterator;
    typedef NeuronNetNodeID2NodeMapTy::const_iterator const_iterator;

    NodeID totalNeuronNatNode;

public:
    /// Constructor
    NeuronNet(): totalNeuronNatNode(0) {}

    /// Destructor
    ~NeuronNet() override;

    /// get a NNGrap node
    inline NeuronNode* getNeuronNetNode(NodeID id) const{
        return getGNode(id);
    }

    /// whether it has a NNNode
    inline bool hasNeuronNetNode(NodeID id) const{
        return hasGNode(id);
    }

    /// whether it has a NNEdge
    NeuronEdge* hasNeuronEdge(NeuronNode* src, NeuronNode* dst, NeuronEdge::NeuronEdgeK kind);

    /// Get a NNGraph edge according to src and dst
    NeuronEdge* getNeuronEdge(const NeuronNode* src, const NeuronNode* dst, NeuronEdge::NeuronEdgeK kind);

    /// Dump graph into dot file
    void dump(const std::string& file, bool simple = false);

    /// View graph from the debugger
    void view();

protected:
    /// Remove an NNEdge, maybe will be used later.
    /// Remove a NNNode, maybe will be used later.

    // Add NNEdge for Node
//    NeuronEdge*  addNNEdge(NeuronNode* srcNode, NeuronNode* dstNode);


    /// Add NNEdge
    inline bool addNeuronEdge(NeuronEdge* edge){
        bool added1 = edge->getDstNode()->addIncomingEdge(edge);
        bool added2 = edge->getSrcNode()->addOutgoingEdge(edge);
        bool all_added = added1 && added2;
        assert(all_added && "NeuronEdge not added?");
        return all_added;
    }

    /// Add NNNode
    virtual inline void addNeuronNode(NeuronNode* node){
        addGNode(node->getId(), node);
    }

private:
    /// Add ReLu Node
    inline ReLuNeuronNode* addReLuNeuronNode( const unsigned in_w, const unsigned in_h, const unsigned in_d){
        ReLuNeuronNode* sNode = new ReLuNeuronNode(totalNeuronNatNode++, in_w, in_h, in_d);
        //NodeID addedNodeID = totalNeuronNatNode;
        addNeuronNode(sNode);
        return sNode;
    }

    /// get ReLu Node, Not using Currently
//    inline ReLuNeuronNode* getaddReLuNeuronNode(const NeuronNode* node){
//        const_iterator it =
//    }


    /// BasicOp layer
    inline BasicOPNeuronNode* addBasicOPNeuronNode(const std::string op, const std::vector<Eigen::MatrixXd>& w, unsigned in_w, unsigned in_h, unsigned in_d){
        BasicOPNeuronNode* sNode = new BasicOPNeuronNode(totalNeuronNatNode++, op, w, in_w, in_h, in_d);
        addNeuronNode(sNode);
        return sNode;
    }

    /// MaxPool layer
    inline MaxPoolNeuronNode* addMaxPoolNeuronNode(NodeID id, unsigned ww, unsigned wh, unsigned sw, unsigned sh, unsigned pw, unsigned ph, unsigned in_w, unsigned in_h, unsigned in_d){
        MaxPoolNeuronNode* sNode = new MaxPoolNeuronNode(totalNeuronNatNode++, ww, wh, sw, sh, pw, ph, in_w, in_h, in_d);

        addNeuronNode(sNode);
        return sNode;
    }

    /// FullyCon layer
    inline FullyConNeuronNode* addFullyConNeuronNode(const Eigen::MatrixXd& w, const Eigen::VectorXd& b, unsigned in_w, unsigned in_h, unsigned in_d){
        FullyConNeuronNode* sNode = new FullyConNeuronNode(totalNeuronNatNode++, w, b, in_w, in_h, in_d);
        addNeuronNode(sNode);
        return sNode;
    }

    /// ConvNeuronNode layer
    inline ConvNeuronNode* addConvNeuronNode(const std::vector<FilterSubNode>& fil, const std::vector<double> b, unsigned in_w, unsigned in_h, unsigned pad, unsigned str){
        ConvNeuronNode* sNode = new ConvNeuronNode(totalNeuronNatNode++, fil, b, in_w, in_h, pad, str);
        addNeuronNode(sNode);
        return sNode;
    }

    /// Constant: Nothing to do OR Input layer
    inline ConstantNeuronNode* addConstantNeuronNode(unsigned iw, unsigned ih, unsigned id){
        ConstantNeuronNode* sNode = new ConstantNeuronNode(totalNeuronNatNode++, iw, ih, id);
        addNeuronNode(sNode);
        return sNode;
    }

};

} // End namespace SVF


namespace SVF{
/* !
 * GenericGraphTraits specializations for generic graph algorithms.
 * Provide graph traits for traversing from a constraint node using standard graph traversals.
 */
template<> struct GenericGraphTraits<SVF::NeuronNode*> : public GenericGraphTraits<SVF::GenericNode<SVF::NeuronNode,SVF::NeuronEdge>*  >
{
};

/// Inverse GenericGraphTraits specializations for call graph node, it is used for inverse traversal.
//template<>
//struct GenericGraphTraits<Inverse<SVF::NeuronNode *> > : public GenericGraphTraits<Inverse<SVF::GenericNode<SVF::NeuronNode,SVF::NeuronEdge>* > >
//{
//};

template<> struct GenericGraphTraits<SVF::NeuronNet*> : public GenericGraphTraits<SVF::GenericGraph<SVF::NeuronNode,SVF::NeuronEdge>* >
{
    typedef SVF::NeuronNode *NodeRef;
};



} // End namespace SVF

#endif // SVF_NNGRAPH_H

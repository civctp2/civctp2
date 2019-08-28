N[$.indegree==0]{ // indegree==0 <=> root-node
    color="#00000055";
    style="filled";
    graph_t sg = subg($T, "root-nodes"); // create an anonymous subgraph for root nodes, or return existing subgraph
    subnode(sg, $); // insert node into sg
}
END_G{
    aset(sg, "rank", "same"); // put root-nodes on same rank
}

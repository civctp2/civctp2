N[$.indegree==0]{ // indegree==0 <=> root-node
    color="#00ff0055";
    style="filled";
    graph_t sg = subg($T, "root-nodes"); // create an anonymous subgraph for root nodes, or return existing subgraph
    subnode(sg, $); // insert node into sg
}
N[$.outdegree==0]{ // outdegree==0 <=> leaf-node
    color="#ff000055";
    style="filled";
}
N[$.indegree!=0 && $.outdegree!=0]{ // intermediate node
    color="#88888855"; // yellow bg looks odd
    style="filled";
}
END_G{
    aset($, "bgcolor", "#111111"); // dark bg
    aset(sg, "rank", "same"); // put root-nodes on same rank
}

N{
    fontcolor="#ffffff88"
}
N[$.indegree==0]{ // indegree==0 <=> root-node
    color="#00ff0055";
    style="filled";
    graph_t sg = subg($T, "root-nodes"); // create an anonymous subgraph for root nodes, or return existing subgraph
    subnode(sg, $); // insert node into sg
}
N[$.xlabel]{
    graph_t sgA = subg($T, "cluster_"+$.xlabel); // create a cluster subgraph for each age
    aset(sgA, "label", $.xlabel);
    subnode(sgA, $); // insert node into sg
}
N[$.outdegree==0]{ // outdegree==0 <=> leaf-node
    color="#ff000055";
    style="filled";
}
N[$.indegree!=0 && $.outdegree!=0]{ // intermediate node
    color="#88888855"; // yellow bg looks odd
    style="filled";
}
E{
    color="#ffffff88"
}
END_G{
    aset($, "bgcolor", "#111111"); // dark bg
    aset($, "margin", "0"); // no margin
    aset($, "pad", "1"); // pad graph [inches]
    aset(sg, "rank", "same"); // put root-nodes on same rank
}

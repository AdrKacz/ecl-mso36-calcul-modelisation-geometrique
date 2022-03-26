void Mesh::removeShortestEdge(){
    unsigned int id_face, id_sommet_oppose;
    findShortestEdge(id_face, id_sommet_oppose);


    //indices des faces à supprimer (IdFaceDelete<Bas/Haut>)
    unsigned int ifdb, ifdh;
    //indices des faces adjacentes (IdFace<Bas/Haut><1/2>)
    unsigned int ifb1, ifb2, ifh1, ifh2;
    //indices des sommets de l'arrete (IdSommet<1/2>)
    unsigned int is1, is2;
    //indices locaux des sommets dans chaque face adjacentes (IdLocalSommet<Bas/Haut><1,2>)
    unsigned int ilsb1, ilsb2, ilsh1, ilsh2;

    // ## Definition des indices ##

    //partie haute
    ifdh = id_face;
    unsigned int id_local_sommet_oppose = get_id_local_sommet(id_face, id_sommet_oppose);
    ilsh1 = (id_local_sommet_oppose+1)%3;
    ilsh2 = (id_local_sommet_oppose+2)%3;
    ifh1 = _faces[ifdh].neighbours[ilsh2];
    ifh2 = _faces[ifdh].neighbours[ilsh1];

    //sommets de l'arrete
    is1 = _faces[ifdh].sommets[ilsh1];
    is2 = _faces[ifdh].sommets[ilsh2];

    //std::cout << "suppression de l'arete " << is1 << "," << is2 << std::endl;
    //partie basse
    ifdb = _faces[ifdh].neighbours[id_local_sommet_oppose];
    ilsb1 = get_id_local_sommet(ifdb, is1);
    ilsb2 = get_id_local_sommet(ifdb, is2);
    ifb1 = _faces[ifdb].neighbours[ilsb2];
    ifb2 = _faces[ifdb].neighbours[ilsb1];

    // ## Modification des voisins ##

    _faces[ifh1].neighbours[(get_id_local_sommet(ifh1, is1)+2)%3] = ifh2;
    _faces[ifh2].neighbours[(get_id_local_sommet(ifh2, is2)+1)%3] = ifh1;
    _faces[ifb1].neighbours[(get_id_local_sommet(ifb1, is1)+1)%3] = ifb2;
    _faces[ifb2].neighbours[(get_id_local_sommet(ifb2, is2)+2)%3] = ifb1;


    //Modification des face rattachés à s2 (s2 -> s1)
    unsigned int current_face = ifb2;
    unsigned int id_local_sommet_s2;
    do{
        id_local_sommet_s2 = get_id_local_sommet(current_face,is2);
        _faces[current_face].sommets[id_local_sommet_s2] = is1;
        //std::cout << "changement triangle " << current_face << std::endl;

        current_face = _faces[current_face].neighbours[(id_local_sommet_s2+1)%3];

    }while(current_face != ifh1);

    //Deplacement du sommet S1
    _sommets[is1].coord = (_sommets[is1].coord + _sommets[is2].coord)/2;

    //Desactivation des faces ifdh/ifdb et du sommet is2
    _faces[ifdh].isEmpty = true;
    _faces[ifdb].isEmpty = true;
    _sommets[is2].isEmpty = true;

}
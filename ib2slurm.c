#include "ib2slurm.h"

/**
 * An iterator for handling each switch in the fabric.
 *
 * @param node the current switch being analyzed.
 * @param user_data various user-controlable options.
 */
void switch_iter_func(ibnd_node_t* node, void* user_data)
{
    ib2slurm_opts_t* opts = (ib2slurm_opts_t*)user_data;

    /*
     * We don't really need to lookup the switch name, slurm will be happy
     * with using the guid of the switch.
     */
    if(opts->lookup_flag) {
        char* switchname = node_name(node, opts);
        fprintf(stdout, "SwitchName=%s", switchname);
        free(switchname);
    } else {
        fprintf(stdout, "SwitchName=%" PRIx64, node->guid);
    }

    /*
     * After printing out the name of the switch, lets print out all of the nodes and
     * switches in the level of the tree directly below the current switch.
     */
    output_nodelist("Switches=", IB_NODE_SWITCH, opts, node);
    output_nodelist("Nodes=", IB_NODE_CA, opts, node);

    fprintf(stdout, "\n");
}

void output_nodelist(char *tag, int type, ib2slurm_opts_t* opts, ibnd_node_t* node)
{
    ibnd_port_t* port;
    int p = 0;

    ib2slurm_list_t list_head;
    list_head.str = NULL;
    list_head.next = NULL;

    ib2slurm_list_t* list_cur = &list_head;

    /*
     * Lets go through all the ports on this switch to see if anything is
     * connected to it.
     */
    for(p = 1; p <= node->numports; p++) {
        port = node->ports[p];

        /*
         * Only print out the node types that match the tag printed
         * before it.
         */
        if(port && port->remoteport && port->remoteport->node->type == type) {

            /* Always attempt a node lookup, since slurm requires it. */
            if(opts->lookup_flag || type == IB_NODE_CA) {
                char* remote = node_name(port->remoteport->node, opts);

                list_cur->str = remote;
                list_cur->next = (ib2slurm_list_t*)malloc(sizeof(ib2slurm_list_t));

                list_cur = list_cur->next;
                list_cur->str = NULL;
                list_cur->next = NULL;
            } else {
                char* buf = (char*)malloc(sizeof(char) * 512);
                sprintf(buf, "%" PRIx64, port->remoteport->guid);

                list_cur->str = buf;
                list_cur->next = (ib2slurm_list_t*)malloc(sizeof(ib2slurm_list_t));

                list_cur = list_cur->next;
                list_cur->str = NULL;
                list_cur->next = NULL;
            }
        }
    }

    if(list_head.str != NULL) {
        fprintf(stdout, " %s", tag);

        for(list_cur = &list_head; list_cur->next != NULL; list_cur = list_cur->next) {
            fprintf(stdout, "%s", list_cur->str);

            if(list_cur->next->str != NULL) {
                fprintf(stdout, ", ");
            }
        }
    }

    /* TODO: free the list elements. */
}

/*
 * A helper to lookup the node name based on the node name map specified by
 * the user.
 *
 * @param the node name to print out.
 * @param opts user specified options.
 */
char* node_name(ibnd_node_t* node, ib2slurm_opts_t* opts)
{
    char* buf = NULL;

    if(node == NULL) {
        fprintf(stderr, "Attempted to get a node name from an invalid node.");
        exit(EXIT_FAILURE);
    }

    buf = remap_node_name(opts->node_name_map, node->guid, node->nodedesc);

    return buf;
}

/*
 * Output a short header to tell someone viewing the topology.conf where it
 * came from.
 */
void output_header()
{
    fprintf(stdout, "# topology.conf\n"
            "# Switch Configuration\n#\n"
            "# Generated by ib2slurm <http://github.com/hpc/ib2slurm>\n#\n");
}

/*
 * Our main entry point.
 */
int main(int argc, char** argv)
{
    ibnd_fabric_t* fabric = NULL;
    char* ibd_ca = NULL;
    int ibd_ca_port = 0;

    char* node_name_map_file = NULL;

    static ib2slurm_opts_t opts;
    int option_index = 0;
    int c;

    static struct option long_options[] = {
        {"node-name-map", required_argument, 0, 'm'},
        {"lookup-switch-name",  no_argument, &opts.lookup_flag,   's'},
        {"compress-node-names", no_argument, &opts.compress_flag, 'c'},
        {0, 0, 0, 0}
    };

    while((c = getopt_long(argc, argv, "m:", long_options, &option_index)) != -1) {
        switch(c) {
            case 'm':
                node_name_map_file = strdup(optarg);
                break;
        }
    }

    opts.node_name_map = open_node_name_map(node_name_map_file);

    if((fabric = ibnd_discover_fabric(ibd_ca, ibd_ca_port, NULL, 0)) == NULL) {
        fprintf(stderr, "IB discover failed.\n");
        exit(EXIT_FAILURE);
    }

    output_header();
    ibnd_iter_nodes_type(fabric, switch_iter_func, IB_NODE_SWITCH, &opts);

    ibnd_destroy_fabric(fabric);
    close_node_name_map(opts.node_name_map);

    exit(EXIT_SUCCESS);
}

/* EOF */

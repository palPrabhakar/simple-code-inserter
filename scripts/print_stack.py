import argparse
from graphviz import Digraph
from collections import defaultdict

colors = defaultdict(lambda: 'lightgray')

colors[0] = 'lightblue'
colors[1] = 'lightgreen'
colors[2] = 'lightyellow'
colors[3] = 'lightpink'
colors[4] = 'lightcoral'
colors[5] = 'lightcyan'


def visualize_leveled_call_graph(edges):
    dot = Digraph()
    # Add nodes and edges to the graph
    for parent, child, lvl in edges:
        parent = parent + '_' + str(lvl-1)
        child = child + '_' + str(lvl)
        dot.node(parent, parent, style='filled', color=colors[lvl])
        dot.node(child, child, style='filled', color=colors[lvl])
        dot.edge(parent, child)
    dot.render('lvl_call_graph', view=True, format='svg')


def visualize_call_graph(edges):
    dot = Digraph()

    # Add nodes and edges to the graph
    for parent, child, lvl in edges:
        dot.node(parent, parent, style='filled', color=colors[lvl])
        dot.node(child, child, style='filled', color=colors[lvl])
        dot.edge(parent, child)

    dot.render('call_graph', view=True, format='png')


def read_stack(file_name):
    edges = []
    stack = [('root', -1)]
    with open(file_name) as f:
        for line in f.readlines():
            if ('end' in line):
                top = stack.pop()
                if (not top[0] == line.split(':')[1].rstrip()):
                    RuntimeError("Invalid Stack")
            else:
                # if (not len(stack) == 0):
                parent = stack[-1]
                edges.append((parent[0], line.rstrip(), parent[1] + 1))
                stack.append((line.rstrip(), parent[1] + 1))

    if (not len(stack) == 0):
        RuntimeError("Invalid Stack")

    return edges


if __name__ == '__main__':
    # Create the parser
    parser = argparse.ArgumentParser()
    parser.add_argument("file", help="path of the stack file")
    parser.add_argument("graph_type", choices=[
                        'simple', 'leveled'], help="Choose a graph type")
    args = parser.parse_args()

    edges = read_stack(args.file)
    if args.graph_type == 'simple':
        visualize_call_graph(edges)
    else:
        visualize_leveled_call_graph(edges)

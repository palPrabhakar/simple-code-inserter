from graphviz import Digraph
from collections import defaultdict

colors = defaultdict(lambda: 'lightgray')

colors[0] = 'lightblue'
colors[1] = 'lightgreen'
colors[2] = 'lightyellow'
colors[3] = 'lightpink'
colors[4] = 'lightcoral'
colors[5] = 'lightcyan'


def visualize_call_graph(edges):
    dot = Digraph()

    # Add nodes and edges to the graph
    for parent, child in edges:
        dot.node(parent, parent, style='filled', color=colors[0])
        dot.node(child, child, style='filled', color=colors[1])
        dot.edge(parent, child)

    dot.render('custom_binary_tree', view=True, format='png')


def read_stack():
    edges = []
    stack = []
    with open('dump.stk') as f:
        for line in f.readlines():
            if ('end' in line):
                top = stack.pop()
                if (not top == line.split(':')[1].rstrip()):
                    RuntimeError("Invalid Stack")
            else:
                if (not len(stack) == 0):
                    parent = stack[-1]
                    edges.append((parent, line.rstrip()))
                stack.append(line.rstrip())

    if (not len(stack) == 0):
        RuntimeError("Invalid Stack")

    return edges


if __name__ == '__main__':
    edges = read_stack()
    visualize_call_graph(edges)

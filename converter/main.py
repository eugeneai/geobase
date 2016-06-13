import csv
import rdflib
from rdflib import Namespace, RDF, XSD, URIRef, BNode, Literal
INPUT="../Active_fault.csv"
OUTPUT="../geobase-server/data/activity_fall_data.ttl"
FORMAT="turtle"

G=rdflib.Graph(identifier="geobase-data")

GEOB=Namespace("http://www.semanticweb.org/bernard_black/ontologies/2016/3/fault#")
G.bind("geob",GEOB)
NIE=Namespace("http://www.semanticdesktop.org/ontologies/2007/01/19/nie#")
G.bind("nie",NIE)
OWL=Namespace("http://www.w3.org/2002/07/owl#")
G.bind("owl",OWL)

class GraphConstructor(object):
    """Constructs Falut data fraph
    """

    def __init__(self, g):
        """

        Arguments:
        - `g`: a rdflib graph to be filled in with data
        """
        self.g = g

    LITERAL_MAP = {
        "id": (NIE, "identifier"),
        "identifier": (NIE, "identifier"),
        "title": (NIE, "title"),
        }

    def fault(self, subj=None, **kwargs):
        if subj==None:
            subj=BNode()

        print (kwargs)
        for k,v in kwargs.items():
            NS, dp=self.__class__.LITERAL_MAP[k]
            self.g.add((subj, NS[dp],Literal(v)))
        return subj


def convert():
    i=open(INPUT)
    # o=open(OUTPUT)
    g=GraphConstructor(G)
    g.fault(
        id="RUAF_1",
        title="Kukus"
    )


    G.serialize(OUTPUT, format=FORMAT)


if __name__=="__main__":
    convert()
    quit()

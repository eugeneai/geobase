import csv
import rdflib
import re
from rdflib import Namespace, RDF, XSD, URIRef, BNode, Literal
INPUT="../Active_fault.csv"
OUTPUT="../geobase-server/data/activity_fall_data.ttl"
FORMAT="turtle"
HLEN=6

CONCEPT=re.compile("^[A-Z][A-Za-z_]*$")
REL=re.compile("^[a-z][A-Za-z_:]*$")

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

    def gather(self, subj, **kwargs):
        print (kwargs)
        for k,v in kwargs.items():
            NS, dp=self.__class__.LITERAL_MAP[k]
            self.g.add((subj, NS[dp],Literal(v)))
        return subj

    def fault(self, subj=None, **kwargs):
#        identifier=kwargs["identif"]
        if subj==None:
            subj=BNode()

        self.gather(subj, **kwargs)

        return subj

    def feed(self, row):
        self.S=[]
        def pred(c):
            for i,r in enumerate(HEADER_L):
                col=r[c]
                r=col.strip()
                if not r:
                    self.S[i]
                    continue
                if CONCEPT.match(col):
                    self.S=self.S[:i]
                    if col=="Fault":
                        self.S.append(self.fault())
                    else:
                        obj=BNode()
                        print (self.S, obj, col, i)
                        subj=self.S[-1]
                        self.g.add((subj, GEOB[col.lower()], obj))
                        self.S.append(obj)
                        continue
                elif REL.match(col):
                    l=col.split(":")
                    if len(l)>1:
                        ns,col=l
                        ns=ns.upper()
                        ns=globals()[ns]
                        return ns[col]
                    return GEOB[col]
                else:
                    raise ValueError("wrong entity '{}'".format(col))

        def col(c):
            val=row[c]

#            import pdb; pdb.set_trace()
            p=pred(c)
            oval=Literal(val)
            self.g.add((self.S[-1], p, oval))

        #for c, val in enumerate(row):
        #    col(c, val)

        for i, _col in enumerate(row):
            col(i)



HEADER_L=None
DATA=None

def convert():
    global HEADER_L
    i=open(INPUT)

    r=csv.reader(i)
    lines=list(r)
    h=HEADER_L=lines[:5]
    DATA=lines[6:]
    g=GraphConstructor(G)


    for row in DATA:
        g.feed(row)
        break

    G.serialize(OUTPUT, format=FORMAT)


if __name__=="__main__":
    convert()
    quit()

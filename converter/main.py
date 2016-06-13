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
FOAF=Namespace("http://xmlns.com/foaf/0.1/")
G.bind("foaf",FOAF)

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
        for k,v in kwargs.items():
            NS, dp=self.__class__.LITERAL_MAP[k]
            self.g.add((subj, NS[dp],Literal(v)))
        return subj

    def fault(self, subj=None, **kwargs):
        if subj==None:
            subj=BNode()

        self.gather(subj, **kwargs)
        self.g.add((subj, RDF["type"], GEOB["Fault"]))

        return subj

    def feed(self, row):
        self.S=[]
        def pred(c):
            for i,r in enumerate(HEADER_L):
                col=r[c]
                col=col.strip()
                if not col:
                    continue
                if CONCEPT.match(col):
                    self.S=self.S[:i]
                    if col=="Fault":
                        self.S.append(self.fault())
                    else:
                        obj=BNode()
                        subj=self.S[-1]
                        prop=col[0].lower()+col[1:]
                        self.g.add((subj, GEOB[col.lower()], obj))
                        self.g.add((obj, RDF["type"], GEOB[col]))
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
                elif col.find("=")>=0:
                    l=col.split("=")
                    prop,val=l
                    self.g.add((self.S[-1], GEOB[prop], GEOB[val]))
                    continue
                else:
                    raise ValueError("wrong entity '{}'".format(col))

        def col(c):
            val=row[c]
            p=pred(c)
            oval=Literal(val)
            subj=self.S[-1]
            #print (subj, p, oval)
            self.g.add((subj, p, oval))

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


    for num, row in enumerate(DATA):
        g.feed(row)
        if num % 100 == 0:
            print (num+1)
        #break

    print ("Serialization")

    G.serialize(OUTPUT, format=FORMAT)


if __name__=="__main__":
    convert()
    quit()

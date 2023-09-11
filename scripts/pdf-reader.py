from pypdf import PdfReader
import string
import nltk
nltk.download('stopwords')
from nltk.corpus import stopwords

def preprocess(text):
    english_stopwords = stopwords.words('english')
    trans = str.maketrans('','', string.punctuation)
    text = text.translate(trans)
    text = text.lower()
    clean_text = ""
    for word in text.split():
        if word not in english_stopwords:
            clean_text += word + " "
    return clean_text


content = ""
with open('/home/l3vi/workspace/big-data/scrappers/quant-ph_0201082v1.Quantum_Computers_and_Quantum_Computer_Languages__Quantum_Assembly_Language_and_Quantum_C_Language.pdf', 'rb') as pdf_file:
    pdf_reader = PdfReader(pdf_file)
    num_pages = len(pdf_reader.pages)
    for page in pdf_reader.pages:
        page_text = page.extract_text()
        content += page_text

print(preprocess(content))
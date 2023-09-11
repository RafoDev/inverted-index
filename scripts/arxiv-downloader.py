import arxiv
import argparse
import string
import nltk
# nltk.download('stopwords')
from nltk.corpus import stopwords
from pypdf import PdfReader
import re
import os

def clean_filename(string):
    char_not_allowed = r'[\/:*?"<>|]'
    replace_dict = {
        '\\': '-',
        '/': '-',
        ':': '-',
        '*': '-',
        '?': '-',
        '"': '-',
        '<': '-',
        '>': '-',
        '|': '-',
        ' ': '-',
    }

    cleaned_filename = re.sub(char_not_allowed, '', string)
    cleaned_filename = cleaned_filename.translate(str.maketrans(replace_dict))
    cleaned_filename = cleaned_filename.lower()

    return cleaned_filename

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


parser = argparse.ArgumentParser("Script to download arxiv papers in pdf")

parser.add_argument('query', type=str, help='Topic, author, tag, etc.')
parser.add_argument('n_results', type=int, default=1, help='Number of results')

args = parser.parse_args()

search = arxiv.Search(
    query=args.query,
    max_results=args.n_results,
)

for result in search.results():
    filename = clean_filename(result.title)
    result.download_pdf(dirpath="../raw/",filename=filename)
    content = ""
    with open("../raw/" + filename, "rb") as pdf_file:
        pdf_reader = PdfReader(pdf_file)
        num_pages = len(pdf_reader.pages)
        for page in pdf_reader.pages:
            page_text = page.extract_text()
            content += page_text
        with open("../files/" + filename, "w") as prepro_file:
            prepro_file.write(content)

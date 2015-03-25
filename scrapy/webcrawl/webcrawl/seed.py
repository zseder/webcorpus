import cStringIO
import gzip
import urllib2
import re

def wp_seed(lang='hu', tld='hu'):
    url = 'https://dumps.wikimedia.org/{0}wiki/latest/{0}wiki-latest-externallinks.sql.gz'.format(lang)  # nopep8
    response = urllib2.urlopen(url)
    zipped_dump = response.read()
    data = cStringIO.StringIO(zipped_dump)
    data = gzip.GzipFile(fileobj=data).read()

    tld_p = re.compile(".{0}[^\w.]?$".format(tld))
    for l in data.split('\n'):
        if not l.startswith('INSERT INTO'):
            continue
        line = l.rstrip(';').split(' ', 4)[4]
        entries = eval("( %s )" % line)
        for e in entries:
            u = e[3]
            if tld_p.search(u):
                yield u

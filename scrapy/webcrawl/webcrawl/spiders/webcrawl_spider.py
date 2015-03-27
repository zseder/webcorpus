import signal

from langid.langid import LanguageIdentifier, model
from bs4 import BeautifulSoup

from scrapy.contrib.linkextractors.lxmlhtml import LxmlLinkExtractor
from scrapy.contrib.spiders import CrawlSpider, Rule
from scrapy.http import Request
from scrapy import log

from webcrawl.items import WebcrawlItem
from webcrawl.seed import wp_seed


class Alarm(Exception):
    pass


def alarm_handler(signum, frame):
    raise Alarm


class WebCrawler(CrawlSpider):
    name = "web"
    allowed_domains = ["hu"]

    def __init__(self, **kwargs):
        super(WebCrawler, self).__init__(**kwargs)
        self.urls = {}
        for url in wp_seed():
            self.add_url(url)
        self.li = LanguageIdentifier.from_modelstring(model, norm_probs=False)
        signal.signal(signal.SIGALRM, alarm_handler)

    rules = (
        Rule(LxmlLinkExtractor(), callback='parse_url', follow=True,
             process_links='process_links'),)

    def parse_url(self, response):
        try:
            if not hasattr(response, 'body_as_unicode'):
                # possibly no text in page
                self.log('no body in {0}'.format(response.url), level=log.INFO)
                return
            signal.alarm(10)

            item = WebcrawlItem()
            item['url'] = response.url
            item['body'] = response.body_as_unicode()

            # langdetect
            #lang = self.li.classify(item['body'])
            #if lang[0] != 'hu':
            #    self.log('not hungarian: {0}'.format(response.url), level=log.INFO)
            #    return

            # html parse with bs4
            #s = BeautifulSoup(item['body'])
            #for script in s(["script", "style"]):
            #    script.extract()
            #item['raw'] = s.get_text()
            #del item['body']

            #self.log('scraped {0} with priority {1}'.format(response.url, self.urls.get(response.url, 0)), level=log.INFO)
            signal.alarm(0)
            return item
        except Alarm as e:
            self.log(repr(e), level=log.ERROR)
            signal.alarm(0)
            return

    def process_links(self, links):
        for link in links:
            self.add_url(link.url)
            yield link

    def add_url(self, url):
        self.urls[url] = self.urls.get(url, 0) + 1

    def make_requests_from_url(self, url):
        return Request(url, priority=self.urls.get(url, 0))

    def start_requests(self):
        for url in self.urls.keys():
            r = self.make_requests_from_url(url)
            yield  r

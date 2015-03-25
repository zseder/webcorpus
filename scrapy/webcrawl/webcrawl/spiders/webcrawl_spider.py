from scrapy.contrib.linkextractors.lxmlhtml import LxmlLinkExtractor
from scrapy.contrib.spiders import CrawlSpider, Rule

from webcrawl.items import WebcrawlItem
from webcrawl.seed import wp_seed

class WebCrawler(CrawlSpider):
    name = "web"
    allowed_domains = ["hu"]
    #start_urls = [
    #    "http://www.index.hu/",
    #    "http://www.origo.hu/",
    #    "http://www.444.hu/",
    #]
    def __init__(self, **kwargs):
        super(WebCrawler, self).__init__(**kwargs)
        self.start_urls = list(wp_seed())

    rules = (Rule(LxmlLinkExtractor(), callback='parse_url', follow=True), )

    def parse_url(self, response):
        if not hasattr(response, 'body_as_unicode'):
            # possibly no text in page
            return

        item = WebcrawlItem()
        item['url'] = response.url
        item['body'] = response.body_as_unicode()
        return item

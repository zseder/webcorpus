from scrapy.contrib.linkextractors.lxmlhtml import LxmlLinkExtractor
from scrapy.contrib.spiders import CrawlSpider, Rule

from webcrawl.items import WebcrawlItem

class WebCrawler(CrawlSpider):
    name = "web"
    allowed_domains = ["hu"]
    start_urls = [
        "http://www.index.hu/",
        "http://www.origo.hu/",
        "http://www.444.hu/",
    ]
    rules = (Rule(LxmlLinkExtractor(), callback='parse_url', follow=True), )

    def parse_url(self, response):
        item = WebcrawlItem()
        item['url'] = response.url
        item['body'] = response.body_as_unicode()
        return item
